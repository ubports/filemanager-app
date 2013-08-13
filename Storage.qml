import QtQuick.LocalStorage 2.0
import QtQuick 2.0

Item {
    property var db: null

    function openDB() {
        if(db !== null) return;

        db = LocalStorage.openDatabaseSync("ubuntu-filemanager-app", "", "Default Ubuntu file manager app", 100000);

        if (db.version === "") {
            db.changeVersion("", "0.1",
                function(tx) {
                    tx.executeSql('CREATE TABLE IF NOT EXISTS settings(key TEXT UNIQUE, value TEXT)');
                    console.log('Database created');
                });
            // reopen database with new version number
            db = LocalStorage.openDatabaseSync("ubuntu-filemanager-app", "", "Default Ubuntu file manager app", 100000);
        }
    }

    function saveSetting(key, value) {
        openDB();
        db.transaction( function(tx){
            tx.executeSql('INSERT OR REPLACE INTO settings VALUES(?, ?)', [key, value]);
        });
    }

    function getSettings(callback) {
        openDB();
        var settings = {};
        db.readTransaction(
            function(tx){
                var rs = tx.executeSql('SELECT key, value FROM Settings');
                for(var i = 0; i < rs.rows.length; i++) {
                    var row = rs.rows.item(i);
                    settings[row.key] = row.value;
                }
                callback(settings);
            }
        );
    }

    function clearSetting(name) {
        openDB();
        db.transaction(function(tx){
            tx.executeSql('DELETE FROM Settings WHERE key = ?', [name]);
        });
    }

    function clearDB() { // for dev purposes
        openDB();
        db.transaction(function(tx){
            tx.executeSql('DELETE FROM Settings WHERE 1');
        });
    }
}
