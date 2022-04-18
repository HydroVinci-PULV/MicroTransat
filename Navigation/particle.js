class Particle {
    constructor(x = 0, y = 0, mass = Math.random()) {
        this.position = { x: x, y: y };
        this.velocity = { x: 0, y: 0 };
        this.acceleration = { x: 0, y: 0 };
        this.positionHistory = [];
        this.speed = 0;
        this.squaredDistance = 0;
        this.mass = 500;
        this.area = 1;
    }
}