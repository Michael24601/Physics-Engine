
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

#include "boundingVolumeHierarchy.h"
#include "boundingSphere.h"

#include "collisionDetector.h"

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

// Particle testing
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

// Tree testing
/*
int main() {

    BoundingVolumeHierarchy<BoundingSphere> tree;

    tree.display();

    RigidBody b;

    Vector3D centre(1, 2, 3);

    BoundingSphere s(centre, 100);

    tree.insert(&b, s);

    tree.display();

    centre = Vector3D(50, -120, 7);

    BoundingSphere s2(centre, 150);

    tree.insert(&b, s2);

    tree.display();

    centre = Vector3D(762, 0, 123);

    BoundingSphere s3(centre, 18);

    tree.insert(&b, s3);

    tree.display();

    return 0;
}
*/

// Body testing
/*
int main() {

    sf::RenderWindow window(sf::VideoMode(800, 800), "Test");

    // Just in order to flip y axis
    sf::View view = window.getDefaultView();
    view.setSize(800, -800);
    window.setView(view);

    sf::Clock clock;
    real deltaT = 0;

    real side = 100;
    real side2 = 60;
    RectangularPrism c(side, side, side, 150, Vector3D(400, 200, 0), sf::Color::Yellow);
    RectangularPrism c2(side2, side2, side2, 60, Vector3D(400, 100, 0), sf::Color::Red);

    real height = 50;
    real radius = 30;
    Cone sp(radius, height, 100, Vector3D(400, 100, 0), sf::Color::Cyan);
    RigidBody fixed;
    fixed.position = Vector3D(400, 700, 0);


    c.body->angularDamping = 0.80;
    c.body->linearDamping= 0.95;

    c2.body->angularDamping = 0.80;
    c2.body->linearDamping = 0.95;

    sp.body->angularDamping = 0.80;
    sp.body->linearDamping = 0.95;

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side/2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 15, 100);

    Vector3D app2(side / 2.0, -side / 2.0, side / 2.0);
    Vector3D app3(-side2 / 2.0, side2 / 2.0, -side2 / 2.0);
    RigidBodySpringForce s2(app3, c.body, app2, 10, 20);
    RigidBodySpringForce s3(app2, c2.body, app3, 10, 20);

    Vector3D app4(-side / 2.0, -side / 2.0, side / 2.0);
    Vector3D app5 = sp.vertices[0];

    RigidBodySpringForce s4(app5, c.body, app4, 10, 10);
    RigidBodySpringForce s5(app4, sp.body, app5, 10, 10);

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                RigidBodyGravity force(Vector3D(3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                RigidBodyGravity force(Vector3D(-3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
        }

        c.body->calculateDerivedData();
        c2.body->calculateDerivedData();
        sp.body->calculateDerivedData();
        fixed.calculateDerivedData();

        s.updateForce((c.body), deltaT);
        s2.updateForce((c2.body), deltaT);
        s3.updateForce((c.body), deltaT);
        s4.updateForce((sp.body), deltaT);
        s5.updateForce((c.body), deltaT);
        g.updateForce((c2.body), deltaT);
        g.updateForce((c.body), deltaT);
        g.updateForce((sp.body), deltaT);

        c.body->integrate(deltaT);
        c.recalculateVertices();
        c2.body->integrate(deltaT);
        c2.recalculateVertices();
        sp.body->integrate(deltaT);
        sp.recalculateVertices();

        Vector3D point = c.body->transformMatrix.transform(app);
        sf::Vector2f p3(point.x, point.y);
        sf::Vector2f p4(fixed.position.x, fixed.position.y);
        sf::VertexArray a(sf::LineStrip, 2);

        a[0].position = p3;
        a[1].position = p4;
        a[0].color = a[1].color = sf::Color::Green;

        point = c2.body->transformMatrix.transform(app3);
        sf::Vector2f p1(point.x, point.y);
        point = c.body->transformMatrix.transform(app2);
        sf::Vector2f p2(point.x, point.y);
        sf::VertexArray b(sf::LineStrip, 2);

        b[0].position = p1;
        b[1].position = p2;
        b[0].color = b[1].color = sf::Color::Green;

        point = sp.body->transformMatrix.transform(app5);
        sf::Vector2f p5(point.x, point.y);
        point = c.body->transformMatrix.transform(app4);
        sf::Vector2f p6(point.x, point.y);
        sf::VertexArray d(sf::LineStrip, 2);

        d[0].position = p5;
        d[1].position = p6;
        d[0].color = d[1].color = sf::Color::Green;

        window.clear(sf::Color::Black);

        vector<sf::VertexArray> v = c.drawLines();
        for (int j = 0; j < v.size(); j++) {
            window.draw(v[j]);
        }

        v = c2.drawLines();
        for (int j = 0; j < v.size(); j++) {
            window.draw(v[j]);
        }

        v = sp.drawLines();
        for (int j = 0; j < v.size(); j++) {
            window.draw(v[j]);
        }

        sf::CircleShape shape;
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(2, 2);
        shape.setRadius(4);
        shape.setPosition(fixed.position.x, fixed.position.y);
        window.draw(shape);

        window.draw(a);
        window.draw(b);
        window.draw(d);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
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

    real side = 100;
    Vector3D prism(side, side, side);
    RectangularPrism c(prism, 150, Vector3D(700, 600, 0), sf::Color::Yellow);

    RigidBody fixed;
    fixed.position = Vector3D(400, 700, 0);


    c.body->angularDamping = 0.8;
    c.body->linearDamping = 0.95;

    RigidBodyGravity g(Vector3D(0, -10, 0));
    Vector3D app(-side / 2.0, side / 2.0, -side / 2.0);
    Vector3D origin;
    RigidBodySpringForce s(app, &fixed, origin, 15, 200);

    ContactData data{};
    data.contacts = new Contact[100];
    CollisionDetector detector;
    Vector3D normal(1, 0, 0);
    normal.normalize();
    real offset = -200;
    Plane p(normal, offset);

    while (window.isOpen()) {

        clock.restart();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                RigidBodyGravity force(Vector3D(3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                RigidBodyGravity force(Vector3D(-3000, 0, 0));
                force.updateForce(c.body, deltaT);
            }
        }

        c.body->calculateDerivedData();
        fixed.calculateDerivedData();

        s.updateForce((c.body), deltaT);
        g.updateForce((c.body), deltaT);

        c.body->integrate(deltaT);
        c.recalculateVertices();

        
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        // convert it to world coordinates
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

        c.body->position = Vector3D(worldPos.x, worldPos.y, 0);
        

        // Checks for a collision
        std::cout << detector.boxAndPlane(c, p, &data) << "\n";

        Vector3D point = c.body->transformMatrix.transform(app);
        sf::Vector2f p3(point.x, point.y);
        sf::Vector2f p4(fixed.position.x, fixed.position.y);
        sf::VertexArray a(sf::LineStrip, 2);

        a[0].position = p3;
        a[1].position = p4;
        a[0].color = a[1].color = sf::Color::Green;

        window.clear(sf::Color::Black);

        vector<sf::VertexArray> v = c.drawLines();
        for (int j = 0; j < v.size(); j++) {
            window.draw(v[j]);
        }

        sf::CircleShape shape;
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(2, 2);
        shape.setRadius(4);
        shape.setPosition(fixed.position.x, fixed.position.y);
        window.draw(shape);

        sf::VertexArray line(sf::LineStrip, 2);
        line[0].position = sf::Vector2f(-offset, 0);
        line[1].position = sf::Vector2f(-offset, 800);
        line[0].color = line[1].color = sf::Color::Cyan;

        window.draw(a);
        window.draw(line);

        window.display();

        deltaT = clock.getElapsedTime().asSeconds() * 10;
    }

    return 0;
}

