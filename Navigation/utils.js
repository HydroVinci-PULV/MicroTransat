function distance(a, b) {
    return Math.sqrt(Math.pow(a.x - b.x, 2) + Math.pow(a.y - b.y, 2));
}
function ceil(a) {
    var r = Math.ceil(a);
    return r == a ? r + 1 : r;
}
module.exports={distance:distance,ceil:ceil};