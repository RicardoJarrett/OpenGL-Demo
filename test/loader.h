#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

class loader {
public:
	GLint tex_id;
	std::vector<glm::vec3> v_list, n_list, f_list, tris, texVs;
	std::vector<glm::vec2> t_list;
	float* triArray, * texArray;
	int* triIndices, * texIndices;
	int triCount;

	loader();

	void load_obj(std::string path);
	void create_tris();
	void create_indexed_tris();
	void find_tri_points(size_t pos, std::string* line, size_t* offset);
	void find_tex_points(size_t pos, std::string* line, size_t* offset);
	void find_f_list(size_t pos, std::string* line, size_t* offset);

	void print_v_list();
	void print_f_list();
	void print_t_list();
	void print_tris();
	void print_texVs();

	void print_triA();
	void print_texA();

	~loader();
};