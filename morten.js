"use strict";

const Realm = require('realm');

const CarSchema = {
  name: 'Car',
  properties: {
    make:  'string',
    model: 'string',
    miles: {type: 'int', default: 0},
  }
};

const schema = [CarSchema];

console.log(schema);

Realm.Sync.setLogLevel('trace');

function uuid() {
    return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
        var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
        return v.toString(16);
    });
}

var realm;

const server = 'http://127.0.0.1:9080';
const username = "Morten_" + uuid();
const password = "morten-password";

function error_handler(sender, error) {
  console.log(error);
}

// SSLVerifyCallback = bool(const std::string& server_address, port_type server_port, const char* pem_data, size_t pem_size, int preverify_ok, int depth);
function ssl_verify_callback(server_address, server_port, pem_data, pem_size, preverify_ok, depth) {
  console.log(server_address);
  console.log(server_port);
  console.log(pem_data);
  console.log(pem_size);
  console.log(preverify_ok);
  console.log(depth);
}


Realm.Sync.User.register(server, username, password, (error, user) => {

  if (error) {
    console.log("Erro:", error);
  } else {
    const config = {
      schema: schema,
      sync: {
        user, 
        url: 'realms://localhost:9443/~/myrealm', 
        error: error_handler,
        validate_ssl: true,
        ssl_verify_callback: ssl_verify_callback
      }
    };

    realm = new Realm(config);
    const session = realm.syncSession;

    setInterval(() => {
      console.log('NOW');
      realm.write(() => {
        realm.create('Car', { make: 'Honda', model: uuid(), miles: 123});
      });
    }, 1000);
  };
})

