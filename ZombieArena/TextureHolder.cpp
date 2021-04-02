/* Singleton class to create a map of String Key and Texture Value that will load and hold up multiple textures
	without having to run out of memory. We are also implementing a class that will use itself in a static mode,
	meaning it will not need to be initialize to be of use for us.
*/

#include "TextureHolder.h"
#include <assert.h> //Include the "Assert freature"

TextureHolder* TextureHolder::m_s_Instance = nullptr;
TextureHolder::TextureHolder() {
	assert(m_s_Instance == nullptr);
	m_s_Instance = this;
}

Texture& TextureHolder::GetTexture(string const& filename) {
	
	//Get a reference to m_Textures using m_s_Instance
	auto& m = m_s_Instance->m_Textures; //auto is the equivalent of map<string, Texture>

	//Create an iterator to hold a key-value-pair (kvp) and search for the required kvp using the passed in file name
	auto keyValuePair = m.find(filename); //auto is the equivalent of map<string, Texture>::iterator

	//did we find a match?
	if (keyValuePair != m.end()) {

		//Yes, return the texture the second part of the kvp, the texture
		return keyValuePair->second;
	}
	else {
		//File name not found create a new key value pair using the file name
		auto& texture = m[filename];

		//Load the texture from file in the usual way
		texture.loadFromFile(filename);

		//Return the texture to the calling code
		return texture;
	}
}