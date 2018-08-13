const http = require('http');
const zlib = require('zlib');

http.createServer((request, response) => {
    let body = [];
    request.on('error', (err) => {
        console.error(err);
    }).on('data', (chunk) => {
        body.push(chunk);
    }).on('end', () => {
        if (request.headers['content-encoding'] == 'gzip') {
            body = zlib.gunzipSync(Buffer.concat(body)).toString();
        }
        else {
            body = Buffer.concat(body).toString();
        }
        response.setHeader('Content-Type', 'application/json');
        response.end('{"id":' + JSON.stringify(body) + '}')
    });
}).listen(5000, '127.0.0.1');
