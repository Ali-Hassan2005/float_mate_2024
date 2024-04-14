const axios = require("axios");
require("dotenv").config();

exports.getAlldata = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/getdata`);
    const data = [
      {
        platform: "EX01",
        time: "2:6:45 UTC",
        pressure: {
          value: 100,
          unit: "kpa",
        },
        depth: {
          value: 0,
          unit: "Meters",
        },
      },
      {
        platform: "EX01",
        time: "2:6:50 UTC",
        pressure: {
          value: 105,
          unit: "kpa",
        },
        depth: {
          value: 0.8,
          unit: "Meters",
        },
      },
      {
        platform: "EX01",
        time: "2:6:55 UTC",
        pressure: {
          value: 110,
          unit: "kpa",
        },
        depth: {
          value: 1.5,
          unit: "Meters",
        },
      },
      {
        platform: "EX01",
        time: "2:7:00 UTC",
        pressure: {
          value: 115,
          unit: "kpa",
        },
        depth: {
          value: 1.9,
          unit: "Meters",
        },
      },
      {
        platform: "EX01",
        time: "2:7:05 UTC",
        pressure: {
          value: 120,
          unit: "kpa",
        },
        depth: {
          value: 2.3,
          unit: "Meters",
        },
      },
      {
        platform: "EX01",
        time: "2:7:10 UTC",
        pressure: {
          value: 125,
          unit: "kpa",
        },
        depth: {
          value: 2.8,
          unit: "Meters",
        },
      },
    ];
    let msg = response.data.msg;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "error fetch" });
  }
};

exports.start = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/start`);
    const data = [];
    let msg = response.data.msg;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "error start" });
  }
};

exports.ping = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/ping`);
    const data = [];
    let msg = response.data.msg;
    res.render("home", { data: data, message: msg });
  } catch (error) {
    res.status(500).render("home", { data: null, message: "no connection" });
  }
};
