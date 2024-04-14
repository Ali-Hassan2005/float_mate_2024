var express = require("express");
const { getAlldata, start, ping } = require("../controller/index.controller");
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

module.exports = router;
