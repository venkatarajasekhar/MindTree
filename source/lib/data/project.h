#ifndef PROJECT_H
#define PROJECT_H
#include "data/nodes/data_node.h"
#include "data/dnspace.h"
#include "data/python/pyexposable.h"

namespace MindTree
{
    
class DNode;

class Project : public PyExposable
{
	Project(std::string filename="") noexcept;

public:
	~Project();

    static Project* instance();
    static Project* create();
    static Project* load(std::string filename);

    void save(); 
    void saveAs(); 
    DNSpace* fromFile(std::string filename);

    std::string getFilename()const;
	void setFilename(std::string value);
	void setRootSpace(DNSpace* value);
	DNSpace* getRootSpace();

    std::string registerNode(DNode *node);
    std::string registerSocket(DSocket *socket);
    std::string registerSocketType(SocketType t);

    void unregisterItem(std::string idname);
    void unregisterItem(void *ptr);

    void* getItem(std::string idname);
    std::string getIDName(void *ptr);
    std::string registerItem(void* ptr, std::string name);

private:
    std::string filename;
    DNSpace *root_scene;
    static Project *_project;

    std::unordered_map<std::string, void*> idNames;
};
} /* MindTree */

#endif // PROJECT_H
