#pragma once
#include <cmath>

namespace Physics {

    public ref class Particle {
    public:
        double posX;
        double posY;
        double velX;
        double velY;
        double mass;
        double size;

        Particle(double x, double y, double vx, double vy, double m, double s) {
            posX = x;
            posY = y;
            velX = vx;
            velY = vy;
            mass = m;
            size = s;
        }

        double Speed() {
            return sqrt(velX * velX + velY * velY);
        }

        double KineticEnergy() {
            return 0.5 * mass * (velX * velX + velY * velY);
        }

        double MomentumX() {
            return mass * velX;
        }

        double MomentumY() {
            return mass * velY;
        }
    };

    public ref class PhysicsEngine {
    public:
        static void ElasticCollision(Particle^ p1, Particle^ p2) {
            double dx = p2->posX - p1->posX;
            double dy = p2->posY - p1->posY;
            double dist = sqrt(dx * dx + dy * dy);

            if (dist < 0.001) return;

            double nx = dx / dist;
            double ny = dy / dist;

            double dvx = p1->velX - p2->velX;
            double dvy = p1->velY - p2->velY;
            double dvn = dvx * nx + dvy * ny;

            if (dvn <= 0.0) return;

            double impulse = 2.0 * dvn / (p1->mass + p2->mass);

            p1->velX -= impulse * p2->mass * nx;
            p1->velY -= impulse * p2->mass * ny;
            p2->velX += impulse * p1->mass * nx;
            p2->velY += impulse * p1->mass * ny;

            double overlap = (p1->size + p2->size) / 2.0 - dist;
            if (overlap > 0.0) {
                double pushX = overlap * nx / 2.0;
                double pushY = overlap * ny / 2.0;
                p1->posX -= pushX;
                p1->posY -= pushY;
                p2->posX += pushX;
                p2->posY += pushY;

                const double DAMPING = 0.999;
                p1->velX *= DAMPING;
                p1->velY *= DAMPING;
                p2->velX *= DAMPING;
                p2->velY *= DAMPING;
            }
        }

        static void WallCollision(Particle^ p, double left, double top, double right, double bottom) {
            if (p->posX <= left) {
                p->posX = left;
                p->velX = abs(p->velX);
            }
            if (p->posX + p->size >= right) {
                p->posX = right - p->size;
                p->velX = -abs(p->velX);
            }
            if (p->posY <= top) {
                p->posY = top;
                p->velY = abs(p->velY);
            }
            if (p->posY + p->size >= bottom) {
                p->posY = bottom - p->size;
                p->velY = -abs(p->velY);
            }
        }

        static void CursorCollision(Particle^ p, double cursorX, double cursorY) {
            double cx = p->posX + p->size / 2.0;
            double cy = p->posY + p->size / 2.0;

            double nx = cx - cursorX;
            double ny = cy - cursorY;
            double dist = sqrt(nx * nx + ny * ny);

            if (dist < 0.001) {
                p->velX = -p->velX;
                p->velY = -p->velY;
                return;
            }

            nx /= dist;
            ny /= dist;

            double vn = p->velX * nx + p->velY * ny;

            if (vn >= 0.0) return;

            p->velX -= 2.0 * vn * nx;
            p->velY -= 2.0 * vn * ny;
        }

        static bool CheckOverlap(Particle^ p1, Particle^ p2) {
            double dx = p1->posX - p2->posX;
            double dy = p1->posY - p2->posY;
            double dist = sqrt(dx * dx + dy * dy);
            double minDist = (p1->size + p2->size) / 2.0;
            return dist < minDist;
        }

        static bool IsCursorOver(Particle^ p, double cursorX, double cursorY) {
            return (cursorX >= p->posX &&
                cursorX <= p->posX + p->size &&
                cursorY >= p->posY &&
                cursorY <= p->posY + p->size);
        }

        static void UpdatePosition(Particle^ p, double dt) {
            p->posX += p->velX * dt;
            p->posY += p->velY * dt;

            
        }
    };
}