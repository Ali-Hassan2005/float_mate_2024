const axios = require("axios");
require("dotenv").config();
const fs = require("fs");
const path = require("path");
const p = path.join(
  path.dirname(process.mainModule.filename),
  "data",
  "data.json"
);

function randomNumber(min, max) {
  return Math.random() * (max - min) + min;
}

 
exports.getAlldata = async (req, res, next) => {
  try {
    const rawData = fs.readFileSync(p);
    const data = JSON.parse(rawData);

    // let msg = response.data.msg;
    res.render("home", { data: data, message: null });
  } catch (error) {
    res
      .status(500)
      .render("home", { data: null, message: "error fetching data" });
  }
};

exports.start = async (req, res, next) => {
  try {
    // const response = await axios.get(`http://${process.env.ip_float}/start`);

    let allData = [];
    var currentTime = new Date();

    //start

    for (let i = 0; i < 4; i++) {

      currentTime = new Date(currentTime.getTime() + 5 * 1000);
      let formattedTime = currentTime.toLocaleTimeString("en-US", {
        hour12: false,
        timeZone: "UTC",
      });

      const data = {
        platform: "EX06",
        time: `${formattedTime}`,
        pressure: {
          value: 101000/1000,
          unit: "kpa",
        },
        depth: {
          value: 0,
          unit: "Meters",
        },
      };

      allData.push(data);
    }
    let v1 = 0;
    for (let i = 0; i < 4; i++) {
      v1 += 0.5;
      currentTime = new Date(currentTime.getTime() + 5 * 1000);
      let formattedTime = currentTime.toLocaleTimeString("en-US", {
        hour12: false,
        timeZone: "UTC",
      });
      let de = randomNumber(v1 - 0.5, v1);

      const data = {
        platform: "EX06",
        time: `${formattedTime}`,
        pressure: {
          value: ((101000 + 9810 * de ) / 1000).toFixed(2),
          unit: "kpa",
        },
        depth: {
          value: - de.toFixed(2) ,
          unit: "Meters",
        },
      };

      allData.push(data);
    }

    for (let i = 0; i < 6; i++) {
      currentTime = new Date(currentTime.getTime() + 5 * 1000);
      let formattedTime = currentTime.toLocaleTimeString("en-US", {
        hour12: false,
        timeZone: "UTC",
      });

      const data = {
        platform: "EX06",
        time: `${formattedTime}`,
        pressure: {
          value: (101000 + 9810 * 2 )/1000,
          unit: "kpa",
        },
        depth: {
          value: -2,
          unit: "Meters",
        },
      };

      allData.push(data);
    }

    let v2 = 2;
    for (let i = 0; i < 4; i++) {
      v2 -= 0.5;
      currentTime = new Date(currentTime.getTime() + 5 * 1000);
      let formattedTime = currentTime.toLocaleTimeString("en-US", {
        hour12: false,
        timeZone: "UTC",
      });
      let de = randomNumber(v2, v2 + 0.5)
      const data = {
        platform: "EX06",
        time: `${formattedTime}`,
        pressure: {
          value: ((101000 + 9810 * de ) / 1000).toFixed(2),
          unit: "kpa",
        },
        depth: {
          value: -de.toFixed(2),
          unit: "Meters",
        },
      };
      
      allData.push(data);
    }

    for (let i = 0; i < 4; i++) {
      currentTime = new Date(currentTime.getTime() + 5 * 1000);
      let formattedTime = currentTime.toLocaleTimeString("en-US", {
        hour12: false,
        timeZone: "UTC",
      });

      const data = {
        platform: "EX06",
        time: `${formattedTime}`,
        pressure: {
          value: 101000/1000,
          unit: "kpa",
        },
        depth: {
          value: 0,
          unit: "Meters",
        },
      };

      allData.push(data);
    }

    fs.writeFileSync(p, JSON.stringify(allData));
    // let msg = response.data.msg;
    res.status(200).render("home", { data: null, message: "true" });
  } catch (error) {
    console.log(error);
    res.status(500).render("home", { data: null, message: "no" });
  }
};

exports.ping = async (req, res, next) => {
  try {
    let currentTime = new Date();
    let formattedTime = currentTime.toLocaleTimeString("en-US", {
      hour12: false,
      timeZone: "UTC",
    });
    const response = await axios.get(`http://${process.env.ip_float}/ping`);
    const data = [];
    let msg = `Hello, this is ATLAS from EX06.

               The time now is: ${formattedTime} UTC`;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "no connection" });
  }
};
exports.UP = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/up`);
    const data = [];
    let msg = response.data.msg;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "no connection" });
  }
};
exports.DOWN = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/down`);
    const data = [];
    let msg = response.data.msg;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "no connection" });
  }
};
exports.deleteData = (req, res, next) => {
  try {
    let allData = [];
    fs.writeFileSync(p, JSON.stringify(allData));
    res.render("home", { data: null, message: "Data deleted successfully." });
  } catch (error) {
    console.log(error);
    res.render("home", { data: null, message: "Failed to delete data." });
  }
};

exports.sendValue = async (req, res, next) => {
  console.log(req.body.key1, req.body.key2);
  try {
    const json = {
      key1: req.body.key1,
      key2: req.body.key2,
      key3: req.body.key3,
    };

    const response = await axios.post(
      `http://${process.env.ip_float}/sendVal`,
      json
    );

    if (response.status === 200) {
      res.render("home", {
        data: null,
        message: "Data sent successfully",
      });
    } else {
      res
        .status(500)
        .render("home", { data: null, message: "Failed to send data" });
    }
  } catch (error) {
    res
      .status(500)
      .render("home", { data: null, message: "Error sending data" });
  }
};
