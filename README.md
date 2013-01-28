Simple Windows C++ Rest Client
==================================================

Author(s)
--------------------------------------
Eddie Willett


Getting Started
--------------------------------------
Included with this project is an example Visual Studio 2012 solution and project.

To include the rest client in your project:
* Include the project in your solution
* Add a reference to the project
* Add to the main projects 'Additional Include Directories' the path "{YOUR GIT PATH}\rest_client\rest_client;"

or just copy the source :)


This client supports both x86 and x64.

For a Json library I recommend Boost ptree[http://www.boost.org/doc/libs/1_52_0/doc/html/property_tree.html]


TODO
--------------------------------------
1. Return response headers
2. Head Request
3. Proxy support
4. Better error code handling
5. Support for sending/receiving wide-character data
