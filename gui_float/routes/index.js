var express = require("express");
const {
  getAlldata,
  start,
  ping,
  sendValue,
  deleteData,
  UP,
  DOWN,
} = require("../controller/index.controller");
var router = express.Router();

/* GET home page. */
router.get("/", function (req, res, next) {
  const data = [];
  let msg = "";
  res.render("home", { title: "Express", data: data, message: msg });
});

router.get("/getData", getAlldata);
router.get("/start", start);
router.get("/ping", ping);
router.get("/deleteData", deleteData);
router.post("/sendData", sendValue);
router.get("/up", UP);
router.get("/down", DOWN);

module.exports = router;
