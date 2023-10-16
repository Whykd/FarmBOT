const express = require("express");
const cors = require("cors");
const { MongoClient } = require("mongodb");
const schedule = require('node-schedule');
require("dotenv").config();
const uri = process.env.URI;
const sport = process.env.SPORT;
const {LedMatrix, GpioMapping} = require('rpi-led-matrix');

//console.log(uri)
const client = new MongoClient(uri);
try {
	client.connect();
} catch (e) {
	console.error(e);
}
const db = client.db("sensdata");
const app = express();
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");
const port = new SerialPort({ path: sport, baudRate: 57600 }, function (err) {
	if (err) {
		return console.log("Error: ", err.message);
	}
});
const parser = port.pipe(new ReadlineParser({ delimiter: "\r\n" }));
const PORT = 4000;

const matrix = initScreen()

let ardCounter = 0;
// let bucketHasWater = false;

app.use(express.json());
app.use(cors());
port.setDefaultEncoding("utf8");
app.listen(PORT, () => {
	console.log(`Server running on port ${PORT}`);
});

app.post("/update", async (req, res) => {
	const pf = req.body.passphrase;
	const data = req.body.data;

	const watertHour = data.WaterHour
	const waterStartMin = data.waterStartMin
	const waterEndMin = data.waterEndMin
	const lightStartHour = data.lightStartHour
	const lightEndHour = data.lightEndHour

	dataOut = {
		wh : watertHour,
		wsm : waterStartMin,
		wem : waterEndMin,
		lsh : lightStartHour,
		leh : lightEndHour
	}

	if (pf == process.env.PASSPHRASE){
		port.write(dataOut)
		res.status(200).send()
	}
	else {
		res.status(401).send("Unathorized")
	}
});

app.put("/data", async (req, res) => {
	dataOut = db.collection("sensdata").find().limit(60)
	res.send(dataOut)
})



parser.on("data", (data) => {
	ardCounter++
	const sens1 = data.sens1
	const sens2 = data.sens2
	const avg = (parseInt(sens1) + parseInt(sens2)) / 2;
	db.collection("sensdata").insertOne
	({
		sens1: sens1,
		sens2: sens2,
		avg: avg,
		timestamp: new Date(),
	});
	const hasWater = data.hasWater

	const curHour = data.curHour
	const curMin = data.curMin

	if (curHour != new Date().getHours() || curMin != new Date().getMinutes()){
		syncClock()
	}

	insertNewData(data,matrix)

});
function syncClock() {
	const date = new Date();
	const hour = date.getHours();
	const min = date.getMinutes();
	const output = {
		hour : hour,
		min : min
	}
	port.write(output);
}
const job = schedule.scheduleJob('1 * * * *', () => { // run every hour at minute 1
    if (ardCounter < 2){
		port.close();
		port.open();
	}
	ardCounter = 0;
});

function insertNewData(newData , matrix){
	matrix.clear();
	//fill(x0: number, y0: number, x1: number, y1: number)
	//fgColor({ r: 255, g: 0, b: 0 })
	//change back to 0, 0, 0, when done
	matrix.fgColor({r:125, g:125, b:125})
	.fill();
	for(let vals = 0; vals < 60; vals++){
	  let avg = newData[vals]["Sensors"]["avg"]
	  let sens1 = newData[vals]["Sensors"]["sens1"]
	  let sens2 = newData[vals]["Sensors"]["sens2"]
	  avg = ((avg * 32)/1024) + 1; //scale value to fit on 32 pixel height
	  sens1 = ((sens1 * 32)/1024) + 1;
	  sens2 = ((sens2 * 32)/1024) + 1;
	  if (sens1 > sens2){
		//fills the matrix in order so they overlap correctly
		matrix.fgColor({ r: 255, g: 173, b: 66 })
		.fill(vals + 2, 0, vals + 2, sens1)
		.fgColor({ r: 164, g: 255, b: 66 })
		.fill(vals + 2, 0, vals + 2, avg)
		.fgColor({ r: 66, g: 135, b: 255 })
		.fill(vals + 2, 0, vals + 2, sens2)
	  }
	  else {
		matrix.fgColor({ r: 255, g: 173, b: 66 })
		.fill(vals + 2, 0, vals + 2, sens2)
		.fgColor({ r: 164, g: 255, b: 66 })
		.fill(vals + 2, 0, vals + 2, avg)
		.fgColor({ r: 66, g: 135, b: 255 })
		.fill(vals + 2, 0, vals + 2, sens1)
	  }
	}
  }
  
  function initScreen(){
	let matrix = new LedMatrix(
	  {
	  ...LedMatrix.defaultMatrixOptions(),
	  rows: 32,
	  cols: 64,
	  chainLength: 1,
	  hardwareMapping: GpioMapping.AdafruitHat,
	  },
	  {
		...LedMatrix.defaultRuntimeOptions(),
		gpioSlowdown: 4,
	  }
	)
	return matrix;
  }
  