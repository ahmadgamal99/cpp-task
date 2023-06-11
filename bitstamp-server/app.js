"use strict";

import http from "http";
import https from "https";
import fs from "fs";
import WebSocket, { WebSocketServer } from 'ws';


const localServer = http.createServer();

const bitstampServer = https.createServer({
    cert: fs.readFileSync('certificate/dafa.pem'),
    key: fs.readFileSync('certificate/dafa.key')
});


const localWs = new WebSocketServer({ server: localServer });
let client;

bitstampServer.listen(function listening() {

    /** get stream from bitstamp **/

    const bitstampWs = new WebSocket("wss://ws.bitstamp.net", {
        rejectUnauthorized: true,
    });

    bitstampWs.on('error', console.error);

    bitstampWs.on('open', function open() {

        const msg = {
            "event": "bts:subscribe",
            "data": {
                "channel": "order_book_btcusdt"
            }
        };

        bitstampWs.send(JSON.stringify(msg));

    });


    bitstampWs.on('message', function message(data, isBinary) {
        const message = isBinary ? data : data.toString();
        console.log("data returned",message);

        if ( client && message )
            client.send(message)
    });

    bitstampWs.on('error', console.error);

    /** get stream from bitstamp **/

});

/** send stream to cpp app **/
localWs.on('connection', function(ws) {

    client = ws;

    console.log('new node has connected');

    ws.on('message', function(data) {
        console.log('>>> ' + data);
    });

    ws.on('close', function() {
        console.log('Connection closed!');
    });

    ws.on('error', function(e) {
    });
});
/** send stream to cpp app **/


const PORT = 3001;
localServer.listen(PORT);
console.log(`Listening on port ${PORT}...`);
