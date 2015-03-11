#define PRINT_TOKEN false
class cat
{
    
    public:
        static void token(std::string message)
        {
            if(PRINT_TOKEN)
            {
                std::cout << "[TOKEN]: " << message << std::endl;
            }
        }
        
        static std::string symtabtype(int index)
        {
            std::string symTabTypeInfo[] = {"INT", "FLOAT", "VOID", "CHAR", "ARRAY", "POINTER"};
            return symTabTypeInfo[index];
        }
        
        static std::string symtabdecltype(int index)
        {
            std::string symTabDeclTypeInfo[] = {"GLOBAL","LOCAL","PARAM"};
            return symTabDeclTypeInfo[index];
        }
        
};
