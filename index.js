const express = require("express");
const request = require('request');


var fs = require('fs');

const app = express();

app.get('/', (req, res) => {
    fs.readFile('index.html', 'utf8', function (err, data) {
        if (err) throw err;
        return res.send(data);
    })
});

statusMap = { "green": "off", "yellow": "off", "red": "off" }

app.get('/swapStatus', (req, res) => {
    console.log("statusMap    : ", statusMap);

    color = req.query.elem;
    console.log("color        : " + color);
    currentStatus = statusMap[color];
    newStatus = (currentStatus == 'off') ? 'on' : 'off';
    statusMap[color] = newStatus;
    if (color == 'green') {
        id = "L-G001";
    } else if (color == 'yellow') {
        id = "L-Y001";
    } else if (color == 'red') {
        id = "L-R001";
    } else {
        return res.send('Invalid action');
    }

    console.log("currentStatus: " + currentStatus);
    console.log("newStatus    : " + newStatus);

    url = "http://192.168.1.107/op=cs?id=" + id + "&action=" + newStatus;

    request.post(url, {
        json: {
            todo: 'Working good'
        }
    }, (error, res, body) => {
        if (error) {
            console.error(error)
            return
        }
        console.log(`statusCode: ${res.statusCode}`)
        console.log(body)
    })

    return res.send('New status ' + newStatus + ' ...');
});

app.listen(3000, '0.0.0.0');

// app.listen(3000, '0.0.0.0', () =>
//     console.log("Server running on port 3000"),
// );

