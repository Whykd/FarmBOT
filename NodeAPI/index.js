const express = require('express');
var cors = require('cors')
const app = express();
const { SerialPort } = require('serialport')
const port = new SerialPort({ path: '/dev/tty-usbserial1', baudRate: 9600 }, function (err) {
    if (err) {
      return console.log('Error: ', err.message)
    }
  })
  
const PORT = 3000;

app.use(express.json());
app.use(cors())

app.get('/test', (req, res) => {
    res.status(200).send('Hello World!');
    console.log(req.query)
});

app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});

app.post('/update', (req, res) => {
    console.log(req.body)
    const pf = req.body.passphrase
    const data = JSON.stringify(req.body.data)
    const ln = data.split("[")[1].split("]")[0].split(",").length

    if (pf && data && ln == 5) {
        res.status(200).send("Data Recieved By API");
    } else{ 
        res.status(400).send("Bad Request");
    }
    console.log(pf)
    console.log(data)
    console.log(ln) 
});