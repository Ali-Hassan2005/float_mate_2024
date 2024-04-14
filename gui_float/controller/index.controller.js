const axios = require("axios");
require("dotenv").config();

exports.getAlldata = async (req, res, next) => {
  try {
    const response = await axios.get(`http://${process.env.ip_float}/getdata`);
    const data = response.data;
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
