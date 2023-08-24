
#include <SFML/Graphics.hpp>
#include <iostream>
#include "vector3D.h"
#include "anchoredSpringForce.h"
#include "particleForceRegistry.h"
#include "particleGravity.h"
#include "particleContact.h"
#include "particleCable.h"
#include "particleRod.h"
#include "rigidBody.h"
#include <vector>

#include "rectangularPrism.h"
#include "pyramid.h"
#include "triangularPrism.h"
#include "cone.h"
#include "sphere.h"
#include "cylinder.h"
#include "tetrahedron.h"

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"

using namespace pe;
using namespace std;


/*
class Vector4D {
public:
    real x, y, z, w;
    Vector4D() {
        x = y = z = w = 0;
    }
    Vector4D(real x, real y, real z, real w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }
};

    class Matrix4x4 {

    public:

        real data[16];

        // No-arg constructor
        Matrix4x4() {
            data[0] = data[1] = data[2] = data[3] = data[4] = data[5]
                = data[6] = data[7] = data[8] = data[9] = data[10]
                = data[11] = data[12] = data[13] = data[14] = data[15] = 0;
        }

        // Argumented constructor 
        Matrix4x4(real d0, real d1, real d2, real d3, real d4, real d5,
            real d6, real d7, real d8, real d9, real d10, real d11,
            real d12, real d13, real d14, real d15) {
            data[0] = d0;	data[1] = d1;	data[2] = d2;
            data[3] = d3;	data[4] = d4;	data[5] = d5;
            data[6] = d6;	data[7] = d7;	data[8] = d8;
            data[9] = d9;	data[10] = d10;	data[11] = d11;
            data[12] = d12;	data[13] = d13;	data[14] = d14;
            data[15] = d15;
        }

   
        Vector4D operator* (const Vector4D& vector) const {
            Vector4D result;

            for (int i = 0; i < 16; i += 4) {
                result.x += data[i] * vector.x;
                result.y += data[i + 1] * vector.y;
                result.z += data[i + 2] * vector.z;
                result.w += data[i + 3] * vector.w;
            }

            return result;
        }
    };

sf::Vector2f threeToTwo(const Vector3D& vector) {
    real f = 100;
    real n = 1;
    real aspect = 1;
    real M_PI = 3.141592;
    Matrix4x4 projection(
        1.0f / (aspect * std::tan(M_PI / 4.0f)), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / std::tan(M_PI / 4.0f), 0.0f, 0.0f,
        0.0f, 0.0f, -(f + n) / (f - n), -1.0f,
        0.0f, 0.0f, -2.0f * f * n / (f - n), 0.0f);
    Vector4D coordinates(vector.x, vector.y, vector.z, 1);
    Vector4D result = projection * coordinates;
    Vector3D transformResult(result.x / result.w, result.y / result.w,
        result.z / result.w);
    return sf::Vector2f(transformResult.x, transformResult.y);
}
*/


int main() {
    
    sf::RenderWindow window(sf::VideoMode(800, 800), "Test");

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0;

    /*
    vector<GeometricShape*> shapes;
    shapes.push_back(new RectangularPrism(130, 60, 80, 20, Vector3D(150, 650, 0), sf::Color::Green));
    shapes.push_back(new TriangularPrism(70, 100, 120, 20, Vector3D(400,650, 0), sf::Color::Red));
    shapes.push_back(new Pyramid(100, 120, 20, Vector3D(650, 650, 0), sf::Color::Blue));
    shapes.push_back(new Tetrahedron(120, 10, Vector3D(150, 400, 0), sf::Color::Magenta));
    shapes.push_back(new RectangularPrism(100, 100, 100, 20, Vector3D(400, 400, 0), sf::Color::Yellow));
    shapes.push_back(new Pyramid(50, 170, 80, Vector3D(650, 400, 0), sf::Color(255, 120, 60)));
    shapes.push_back(new Cone(60, 120, 15, Vector3D(150, 150, 0), sf::Color::White));
    shapes.push_back(new Sphere(70, 80000, Vector3D(400, 150, 0), sf::Color::Cyan));
    shapes.push_back(new Cylinder(60, 100, 20, Vector3D(650, 150, 0), sf::Color(120, 80, 200)));

    for (int i = 0; i < shapes.size(); i++) {
        shapes[i]->body.addTorque(Vector3D(100, -100, -100));
        shapes[i]->body.calculateDerivedData();
        shapes[i]->body.integrate(10);
    }
    */

    real side = 200;
    RectangularPrism c(side, side, side, 100, Vector3D(400, 400, 0), sf::Color::Yellow);
    RigidBody fixed;
    fixed.position = Vector3D(200, 700, 0);

    RigidBody fixed2;
    fixed2.position = Vector3D(600, 700, 0);

    c.body.angularDamping = 0.98;
    c.body.linearDamping= 0.98;
    
    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side/2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 3, 300);

    Vector3D app2(side / 2.0, side / 2.0, -side / 2.0);
    RigidBodySpringForce s2(app2, &fixed2, origin, 7, 300);

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        /*
        for (int i = 0; i < shapes.size(); i++) {
            shapes[i]->body.calculateDerivedData();
            shapes[i]->body.integrate(deltaT);
            shapes[i]->recalculateVertices();
        }
        */

        c.body.calculateDerivedData();
        fixed.calculateDerivedData();
        fixed2.calculateDerivedData();
       
        s.updateForce(&(c.body), deltaT);
        s2.updateForce(&(c.body), deltaT);
        g.updateForce(&(c.body), deltaT);

        c.body.integrate(deltaT);

        c.recalculateVertices();

        Vector3D point = c.body.transformMatrix.transform(app);
        sf::Vector2f p1(point.x, point.y);
        sf::Vector2f p2(fixed.position.x, fixed.position.y);
        sf::VertexArray a(sf::LineStrip, 2);

        a[0].position = p1;
        a[1].position = p2;
        a[0].color = a[1].color = sf::Color::Green;

        point = c.body.transformMatrix.transform(app2);
        sf::Vector2f p3(point.x, point.y);
        sf::Vector2f p4(fixed2.position.x, fixed2.position.y);
        sf::VertexArray b(sf::LineStrip, 2);

        b[0].position = p3;
        b[1].position = p4;
        b[0].color = b[1].color = sf::Color::Green;

        window.clear(sf::Color::Black);

        /*
        for (int i = 0; i < shapes.size(); i++) {
            vector<sf::VertexArray> v = shapes[i]->drawLines();
            for (int j = 0; j < v.size(); j++) {
                window.draw(v[j]);
            }
        }
        */

        vector<sf::VertexArray> v = c.drawLines();
        for (int j = 0; j < v.size(); j++) {
            window.draw(v[j]);
        }

        sf::CircleShape shape;
        shape.setFillColor(sf::Color::Red);
        shape.setRadius(5);
        shape.setPosition(fixed.position.x, fixed.position.y);
        window.draw(shape);

        sf::CircleShape shape2;
        shape2.setFillColor(sf::Color::Red);
        shape2.setRadius(5);
        shape2.setPosition(fixed2.position.x, fixed2.position.y);
        window.draw(shape2);

        window.draw(a);
        window.draw(b);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}


/*
int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 500), "Test");

    Particle particle[5];
    sf::CircleShape circle[5];
    for (int i = 0; i < 5; i++) {
        circle[i].setRadius(10);
        circle[i].setOrigin(10, 10);
        circle[i].setFillColor(sf::Color::Blue);
        particle[i].setMass(10);
        particle[i].damping = 0.9;
        Vector3D position (100 + i * 60, 400, 0);
        particle[i].position = position;
        circle[i].setPosition(sf::Vector2f(particle[i].position.x, 
            500 - particle[i].position.y));
    }


    Vector3D g(0, -10, 0);
    ParticleGravity gravity(g);

    real deltaT = 0;
    sf::Clock clock;

    ParticleForceRegistry registry;
    for(int i = 0; i < 5; i++)
        registry.addForce(&particle[i], &gravity);

    particle[0].inverseMass = 0;
    particle[4].inverseMass = 0;
    ParticleRod rod [4];
    for (int i = 0; i < 4; i++) {
        rod[i].particle[0] = &particle[i];
        rod[i].particle[1] = &particle[i+1];
        rod[i].length = 100;
    }

    Vector3D force(10000, 0, 0);
    ParticleGravity pushForce(force);
    Vector3D force2(-10000, 0, 0);
    ParticleGravity pushForceLeft(force2);

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                pushForce.updateForce(&particle[3], deltaT);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                pushForceLeft.updateForce(&particle[3], deltaT);
            }
            
        }
        registry.updateAll(deltaT);
            
        for(int i =0; i < 5; i++)
            particle[i].integrate(deltaT);

        ParticleContact collision;
        for (int i = 0; i < 4; i++)
            if (rod[i].fillContact(&collision, 10) > 0) {
                collision.resolveContact(deltaT);
            }

        for (int i = 0; i < 5; i++)
            circle[i].setPosition(particle[i].position.x, 
                500 - particle[i].position.y);

        window.clear(sf::Color::Black);

        for (int i = 0; i < 5; i++) {
            window.draw(circle[i]);
        }

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}*/