
#ifndef SIMULATIONS_H
#define SIMULATIONS_H

// Must be before any SFML or glfw or glm or glew files                        
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtx/string_cast.hpp>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <iostream>
#include <vector>
#include <random>

#include "particle.h"
#include "particleSpringForce.h"
#include "particleBungeeForce.h"
#include "anchoredSpringForce.h"
#include "particleGravity.h"
#include "particleRod.h"
#include "particleCable.h"

#include "rigidBodyGravity.h"
#include "rigidBodySpringForce.h"
#include "boundingVolumeHierarchy.h"

#include "rectangularPrism.h"
#include "solidSphere.h"
#include "pyramid.h"
#include "cylinder.h"
#include "contactGeneration.h"
#include "drawingUtil.h"
#include "cone.h"

#include "solidColorShader.h"
#include "diffuseLightingShader.h"
#include "phongShader.h"
#include "cookTorranceShader.h"
#include "shaderInterface.h"
#include "anisotropicShader.h"
#include "cookTorranceTextureShader.h"
#include "anisotropicTextureShader.h"

#include "sat.h"
#include "contact.h"
#include "contactGeneration.h"
#include "collisionResolver.h"

#include "BVHNode.h"
#include "boundingVolumeHierarchy.h"
#include "boundingSphere.h"

#include "cloth.h"

#include "fixedJoint.h"
#include "springJoint.h"
#include "joint.h"
#include "ballJoint.h"
#include "sphericalJoint.h"

#include "openglUtility.h"
#include "fineCollisionDetection.h"
#include "gjk.h"

#include "cookTorranceReflectionShader.h"
#include "customPrimitive.h"
#include "breakable.h"

#include "freeMovingCamera.h"
#include "rotatingCamera.h"

#include "particleCollisionDetection.h"
#include "particleContactResolver.h"

#include "blob.h"
#include "skyboxShader.h"
#include "cookTorranceSkyboxReflectionShader.h"
#include "shadowMappingShader.h"
#include "simpleShader.h"

#include "environmentMapper.h"
#include "depthMapper.h"
#include "glfwWindowWrapper.h"
#include "diffuseTextureShader.h"

#include "firstPersonCamera.h"
#include "directionalProjection.h"
#include "pointProjection.h"

#include "textureShader.h"
#include "advancedDiffuseTextureShader.h"
#include "shadowMappingTextureShader.h"


namespace pe {

	void runWreckingBall();

	void runStaircase();

	void runBallPit();

	void runRagdoll();

	void runFlag();

	void runReflection();

	void runVideoGameLevel();

}

#endif