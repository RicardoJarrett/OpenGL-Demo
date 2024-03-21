#include "loader.h"

#include <fstream>
#include <iostream>

loader::loader() {
	triArray = nullptr;
	triIndices = nullptr;
	texArray = nullptr;
	texIndices = nullptr;
	triCount = 0;
	tex_id = -1;
}

loader::~loader() {
	delete[] triArray;
	delete[] texArray;
}

void loader::find_tri_points(size_t pos, std::string* line, size_t* offset) {
	glm::vec3 tVec;
	std::string item;
	int index = 0;
	while (pos != std::string::npos) {
		pos = (*line).find(' ', *offset);
		item = (*line).substr(*offset, pos - *offset);
		tVec[index] = std::stof(item);
		*offset = pos + 1;
		index += 1;
	}
	v_list.push_back(tVec);
}

void loader::find_tex_points(size_t pos, std::string* line, size_t* offset) {
	glm::vec2 tVec;
	std::string item;
	int index = 0;
	while (pos != std::string::npos) {
		pos = (*line).find(' ', *offset);
		item = (*line).substr(*offset, pos - *offset);
		tVec[index] = std::stof(item);
		*offset = pos + 1;
		index += 1;
	}
	t_list.push_back(tVec);
}

void loader::find_f_list(size_t pos, std::string* line, size_t* offset) {
	glm::vec3 vPoints;
	glm::vec3 fPoints;
	std::string face;
	int index = 0;
	while (pos != std::string::npos) {
		pos = (*line).find(' ', *offset);
		face = (*line).substr(*offset, pos - *offset);

		vPoints[index] = std::stof(face.substr(0, face.find('/')));
		fPoints[index] = std::stof(face.substr(face.find('/') + 1, face.find('/', face.find('/') + 1)));
		*offset = pos + 1;
		index += 1;
	}
	tris.push_back(vPoints);
	texVs.push_back(fPoints);
}

void loader::load_obj(std::string path) {
	std::ifstream ifile(path);
	if (ifile.is_open()) {
		std::string line;
		while (std::getline(ifile, line)) {
			std::string item;
			size_t pos = line.find(' ');
			if (pos != std::string::npos) {
				item = line.substr(0, pos);
			}

			size_t offset = pos + 1;

			std::vector<glm::vec3>* list = nullptr;
			if (item == "v") {
				find_tri_points(pos, &line, &offset);
			}
			else if (item == "vn") {
				//list = &n_list;
			}
			else if (item == "vt") {
				find_tex_points(pos, &line, &offset);
			}
			else if (item == "f") {
				find_f_list(pos, &line, &offset);
			}
		}
		create_tris();
	}
	else {
		std::cout << "Failed to open: " << path << "\n";
	}
	ifile.close();
}

void loader::create_tris() {
	float scale = 0.5f;

	triCount = tris.size();
	triArray = new float[tris.size() * 9];
	texArray = new float[tris.size() * 6];
	int arrayIndex = 0;
	for (std::vector<glm::vec3>::iterator it = tris.begin(); it != tris.end(); it++) {
		for (int i = 0; i < 3; i++) {
			int current_v = (*it)[i] - 1;
			for (int j = 0; j < 3; j++) {
				triArray[(arrayIndex * 9) + (i * 3) + j] = v_list[current_v][j] * scale;
			}
		}
		arrayIndex++;
	}
	arrayIndex = 0;
	for (std::vector<glm::vec3>::iterator it = texVs.begin(); it != texVs.end(); it++) {
		for (int i = 0; i < 3; i++) {
			int current_t = (*it)[i] - 1;
			for (int j = 0; j < 2; j++) {
				texArray[(arrayIndex * 6) + (i * 2) + j] = t_list[current_t][j];
			}
		}
		arrayIndex++;
	}
}

void loader::create_indexed_tris() {
	triCount = tris.size();
	triArray = new float[v_list.size() * 3];
	triIndices = new int[tris.size() * 3];
	texArray = new float[t_list.size() * 2];
	texIndices = new int[texVs.size() * 3];

	int arrayIndex = 0;
	for (std::vector<glm::vec3>::iterator it = tris.begin(); it != tris.end(); it++){
		for (int i = 0; i < 3; i++) {
			triIndices[arrayIndex + i] = tris[arrayIndex][i];
		}
		arrayIndex++;
	}

	arrayIndex = 0;
	for (std::vector<glm::vec3>::iterator it = texVs.begin(); it != texVs.end(); it++) {
		for (int i = 0; i < 2; i++) {
			texIndices[arrayIndex + i] = texVs[arrayIndex][i];
		}
		arrayIndex++;
	}
	
	for (int arrayIndex = 0; arrayIndex < v_list.size(); arrayIndex++) {
		for (int i = 0; i < 3; i++) {
			triArray[arrayIndex + i] = v_list[arrayIndex][i] * 0.5f;
		}
		arrayIndex++;
	}

	for (int arrayIndex = 0; arrayIndex < t_list.size(); arrayIndex++) {
		for (int i = 0; i < 2; i++) {
			texArray[arrayIndex + i] = t_list[arrayIndex][i];
		}
		arrayIndex++;
	}
}

void loader::print_v_list() {
	std::cout << "V List:\n";
	for (std::vector<glm::vec3>::iterator it = v_list.begin(); it != v_list.end(); it++) {
		std::cout << (*it)[0] << ", " << (*it)[1] << ", " << (*it)[2] << '\n';
	}
}
void loader::print_t_list() {
	std::cout << "T List:\n";
	for (std::vector<glm::vec2>::iterator it = t_list.begin(); it != t_list.end(); it++) {
		std::cout << (*it)[0] << ", " << (*it)[1] << '\n';
	}
}

void loader::print_f_list() {
	std::cout << "F List:\n";
	for (std::vector<glm::vec3>::iterator it = f_list.begin(); it != f_list.end(); it++) {
		std::cout << (*it)[0] << ", " << (*it)[1] << ", " << (*it)[2] << '\n';
	}
}
void loader::print_tris() {
	std::cout << "Tris:\n";
	for (std::vector<glm::vec3>::iterator it = tris.begin(); it != tris.end(); it++) {
		std::cout << (*it)[0] << ", " << (*it)[1] << ", " << (*it)[2] << '\n';
	}
}
void loader::print_texVs() {
	std::cout << "TexVs:\n";
	for (std::vector<glm::vec3>::iterator it = texVs.begin(); it != texVs.end(); it++) {
		std::cout << (*it)[0] << ", " << (*it)[1] << ", " << (*it)[2] << '\n';
	}
}

void loader::print_triA() {
	if (triArray != nullptr) {
		std::cout << "TriA:\n";
		for (int i = 0; i < (triCount * 9); i++) {
			if ((i % 3) == 0) {
				std::cout << '\n' << (i / 3) << "  ";
			}
			std::cout << triArray[i] << " ";
		}
	}
}
void loader::print_texA() {
	if (texArray != nullptr) {
		std::cout << "TexA\n";
		for (int i = 0; i < (triCount * 6); i++) {
			if ((i % 2) == 0) {
				std::cout << '\n' << (i / 2) << "  ";
			}
			std::cout << texArray[i] << " ";
		}
	}
}
