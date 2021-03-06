#include <fstream>
#include <iostream>
#include "ShaderManager.h"

//------------------------------------------------------------------------------------------------------
//constructor that assigns all default values 
//------------------------------------------------------------------------------------------------------
ShaderManager::ShaderManager()
{

	m_program = 0;
	
}
//------------------------------------------------------------------------------------------------------
//getter function that returns the ID of a shader uniform variable based on name passed
//------------------------------------------------------------------------------------------------------
GLint ShaderManager::GetUniform(const std::string& name)
{

	return glGetUniformLocation(m_program, name.c_str());

}
//------------------------------------------------------------------------------------------------------
//getter function that returns the ID of a shader attribute variable based on name passed
//------------------------------------------------------------------------------------------------------
GLint ShaderManager::GetAttribute(const std::string& name)
{

	return glGetAttribLocation(m_program, name.c_str());

}
//------------------------------------------------------------------------------------------------------
//setter function that sends data to a shader uniform variable based on ID passed
//------------------------------------------------------------------------------------------------------
void ShaderManager::SetUniform(GLint attributeID, GLfloat* data)
{

	glUniformMatrix4fv(attributeID, 1, GL_FALSE, data);

}
//------------------------------------------------------------------------------------------------------
//setter function that links data to a shader attribute variable based on ID passed
//------------------------------------------------------------------------------------------------------
void ShaderManager::SetAttribute(GLint attributeID, GLint componentSize)
{

	glVertexAttribPointer(attributeID, componentSize, GL_FLOAT, GL_FALSE, 0, 0);

}
//------------------------------------------------------------------------------------------------------
//function that will activate shader attribute variable based on ID passed
//------------------------------------------------------------------------------------------------------
void ShaderManager::EnableAttribute(GLint attributeID)
{

	glEnableVertexAttribArray(attributeID);

}
//------------------------------------------------------------------------------------------------------
//function that will de-activate shader attribute variable based on ID passed
//------------------------------------------------------------------------------------------------------
void ShaderManager::DisableAttribute(GLint attributeID)
{

	glDisableVertexAttribArray(attributeID);

}
//------------------------------------------------------------------------------------------------------
//function that creates the main shader program
//------------------------------------------------------------------------------------------------------
bool ShaderManager::Initialize()
{

	//create main shader program object
	m_program = glCreateProgram();

	//if an error occured a large unsigned int will be returned 
	//which is converted into a -1 in signed int terms
	if (m_program == -1)
	{
		std::cout << "Shader program could not be created." << std::endl;
		return false;
	}

	return true;
	
}
//------------------------------------------------------------------------------------------------------
//function that creates a vertex, fragment or geometry shader object
//------------------------------------------------------------------------------------------------------
bool ShaderManager::Create(ShaderType shaderType, const std::string& mapIndex)
{

	//temp ID that will store generated shader ID number
	GLint ID = 0;

	//temp flag and storage map which will reference specific shader 
	//creation value and point to specific map that the ID will be stored in. 
	GLenum tempFlag = 0;
	std::map<std::string, GLuint>* tempMap = 0;

	//based on enum type passed assign correct OpenGL flag and storage 
	//map so that when the shader ID is generated and stored the correct 
	//flag and map are used preventing duplicate code
	switch (shaderType) 
	{

		case VERTEX_SHADER:
		{
			tempFlag = GL_VERTEX_SHADER;
			tempMap = &m_vertexShaderIDMap;
			break;
		}

		case FRAGMENT_SHADER:
		{
			tempFlag = GL_FRAGMENT_SHADER;
			tempMap = &m_fragmentShaderIDMap;
			break;
		}

		case GEOMETRY_SHADER:
		{
			tempFlag = GL_GEOMETRY_SHADER;
			tempMap = &m_geometryShaderIDMap;
			break;
		}

	}
			
	//generate a shader ID for specific shader 
	ID = glCreateShader(tempFlag);

	//if an error occured a large unsigned int will be 
	//returned which is converted into a -1 in signed int terms
	//TODO : be more specific about which shader has an error!!!
	if (ID == -1)
	{
		std::cout << "Shader could not be created." << std::endl;
		return false;
	}

	//store ID in specific map
	(*tempMap)[mapIndex] = ID;

	return true;

}
//------------------------------------------------------------------------------------------------------
//function that attaches a vertex, fragment or geometry shader object to the main shader program 
//------------------------------------------------------------------------------------------------------
void ShaderManager::Attach(ShaderType shaderType, const std::string& mapIndex)
{

	//temp pointer which will reference specific shader ID map
	std::map<std::string, GLuint>* tempMap = 0;

	//based on which shader needs to be attached point to specific 
	//storage map so that when attaching later there is no duplicate code
	switch (shaderType)
	{
		case VERTEX_SHADER   : tempMap = &m_vertexShaderIDMap; break;
		case FRAGMENT_SHADER : tempMap = &m_fragmentShaderIDMap; break;
		case GEOMETRY_SHADER : tempMap = &m_geometryShaderIDMap; break;
	}

	//attach shader object using specific storage map
	glAttachShader(m_program, (*tempMap)[mapIndex]);

}
//------------------------------------------------------------------------------------------------------
//function that reads in the shader source code and compiles it
//------------------------------------------------------------------------------------------------------
bool ShaderManager::Compile(ShaderType shaderType, 
	                        const std::string& filename, const std::string& mapIndex)
{

	//variables for file handling, source code 
	//text reading and storing and temp shader ID storage
	std::fstream file;
	std::string lineText = "";
	std::string finalString;
	GLint tempShaderID;
	GLint compileResult;

	//display text to state that file is being opened and read
	std::cout << "Opening and reading " 
		      << (shaderType == VERTEX_SHADER ? "vertex" : "fragment") << " shader file : " 
		      << "\"" << filename << "\"" << std::endl;

	//open shader file
	file.open(filename);

	//if file has an error opening output error message
	if (!file)
	{
		std::cout << "File could not be loaded." << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;
		return false;
	}

	//based on which shader needs to be compiled point to specific 
	//shader ID so that when compiling later there is no duplicate code
	switch (shaderType)
	{
		case VERTEX_SHADER   : tempShaderID = m_vertexShaderIDMap[mapIndex]; break;
		case FRAGMENT_SHADER : tempShaderID = m_fragmentShaderIDMap[mapIndex]; break;
		case GEOMETRY_SHADER : tempShaderID = m_geometryShaderIDMap[mapIndex]; break;
	}

	//read through the entire file and add each line of code into a big string
	//the newline character needs to be added to the end of each code line
	while (!file.eof())
	{
		getline(file, lineText);
		finalString += lineText + "\n";
	}

	//close the file 
	file.close();

	//convert final code string into a char array 
	//because OpenGL requires this format for compilation 
	const GLchar* finalCode = static_cast<const GLchar*>(finalString.c_str());

	//debug output of long shader source code
	std::cout << "Compiling shader file..." << std::endl;
	
	//bind shader object with the shader source code
	glShaderSource(tempShaderID, 1, &finalCode, NULL);

	//compile the source code using the shader object
	glCompileShader(tempShaderID);

	//request compilation error code for error checking
	glGetShaderiv(tempShaderID, GL_COMPILE_STATUS, &compileResult);

	//if compilation went well, display a friendly message
	if (compileResult == GL_TRUE)
	{
		std::cout << "Shader file compiled successfully." << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;
	}

	//otherwise request error string and store in an 
	//array of a set size and display the error on screen
	else
	{
		GLchar error[1000];
		GLsizei length = 1000;

		std::cout << "Shader file could not be compiled. See error list below." << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;

		glGetShaderInfoLog(tempShaderID, 1000, &length, error);
		std::cout << error << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;

		return false;
	}

	return true;

}
//------------------------------------------------------------------------------------------------------
//function that links entire program together
//------------------------------------------------------------------------------------------------------
bool ShaderManager::Link()
{

	//variable to store link error code
	GLint linkResult = 0;

	//link entire shader program with all its shader objects
	glLinkProgram(m_program);

	//activate main shader program
	glUseProgram(m_program);

	//request linking error code for error checking
	glGetProgramiv(m_program, GL_LINK_STATUS, &linkResult);

	//there is no check for link success because when linking is performed 
	//multiple times, that will display too many messages on the console screen

	//if linking failed, request error string and store 
	//in an array of a set size, and display the error on screen
	if (linkResult == GL_FALSE)
	{
		GLchar error[1000];
		GLsizei length = 1000;

		std::cout << "---------------------------------------------------------------" << std::endl;
		std::cout << "Linking : FAIL" << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;

		glGetProgramInfoLog(m_program, 1000, &length, error);
		std::cout << error << std::endl;
		std::cout << "---------------------------------------------------------------" << std::endl;

		return false;
	}

	return true;

}
//------------------------------------------------------------------------------------------------------
//function that detaches a vertex, fragment or geometry shader object from the main shader program
//------------------------------------------------------------------------------------------------------
void ShaderManager::Detach(ShaderType shaderType, const std::string& mapIndex)
{

	//temp pointer which will reference specific shader ID map
	std::map<std::string, GLuint>* tempMap = 0;

	//based on which shader needs to be detached point to specific 
	//storage map so that when detaching later there is no duplicate code
	switch (shaderType)
	{
		case VERTEX_SHADER   : tempMap = &m_vertexShaderIDMap; break;
		case FRAGMENT_SHADER : tempMap = &m_fragmentShaderIDMap; break;
		case GEOMETRY_SHADER : tempMap = &m_geometryShaderIDMap; break;
	}

	//detach shader object using specific storage map
	glDetachShader(m_program, (*tempMap)[mapIndex]);

}
//------------------------------------------------------------------------------------------------------
//function that unloads a single or multiple shader IDs from memory
//------------------------------------------------------------------------------------------------------
void ShaderManager::Destroy(ShaderType shaderType, RemoveType removeType, const std::string& mapIndex)
{

	//temp pointer which will reference specific shader ID map
	std::map<std::string, GLuint>* tempMap = 0;

	//assign whichever shader ID map needs to be removed from 
	//to temp pointer so that when looping through the map later, 
	//the pointer is used instead of looping through three different maps
	switch (shaderType)
	{
		case VERTEX_SHADER   : tempMap = &m_vertexShaderIDMap; break;
		case FRAGMENT_SHADER : tempMap = &m_fragmentShaderIDMap; break;
		case GEOMETRY_SHADER : tempMap = &m_geometryShaderIDMap; break;
	}

	//loop through entire shader ID map in order 
	//to remove a specific shader ID or all shader IDs
	for (auto it = tempMap->begin(); it != tempMap->end(); it++)
	{

		//if a flag is passed to remove a specific shader ID
		//check if map index is the shader that needs to be removed
		//and remove it from both OpenGL and the map
		if (removeType == CUSTOM_SHADER)
		{
			if (it->first == mapIndex)
			{
				glDeleteShader(it->second);
				tempMap->erase(it);
				break;
			}
		}

		//otherwise if a flag is passed to remove all shader IDs
		//remove the shader from OpenGL
		else if (removeType == ALL_SHADERS)
		{
			glDeleteShader(it->second);
		}

	}

	//if all shader IDs have been removed from OpenGL, clear the 
	//entire map in one go, because the IDs in the map still remain
	if (removeType == ALL_SHADERS)
	{
		tempMap->clear();
	}

}
//------------------------------------------------------------------------------------------------------
//function that destroys shader program 
//------------------------------------------------------------------------------------------------------
void ShaderManager::ShutDown()
{

	glDeleteProgram(m_program);
	m_program = 0;

}
//------------------------------------------------------------------------------------------------------
//function that displays total size of all shader ID maps
//------------------------------------------------------------------------------------------------------
void ShaderManager::Output()
{

	system("cls");
	std::cout << "------------------------------------" << std::endl;
	std::cout << "Total size of all shader maps : " << std::endl;
	std::cout << "------------------------------------" << std::endl;
	std::cout << "Size of Vertex Shader ID Map   : " << m_vertexShaderIDMap.size() << std::endl;
	std::cout << "Size of Fragment Shader ID Map : " << m_fragmentShaderIDMap.size() << std::endl;
	std::cout << "Size of Geometry Shader ID Map : " << m_geometryShaderIDMap.size() << std::endl;
	std::cout << "------------------------------------" << std::endl;

}