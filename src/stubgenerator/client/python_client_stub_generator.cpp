#include "python_client_stub_generator.h"
#include <algorithm>

#define TEMPLATE_PYTHON_CLIENT_SIGCLASS "class <stubname>(client.Client):"

#define TEMPLATE_PYTHON_CLIENT_SIGCONSTRUCTOR "def __init__(self, connector, version='2.0'):\n    super(<stubname>, self).__init__(connector, version)"

#define TEMPLATE_PYTHON_CLIENT_SIGMETHOD "def <methodname>(<parameters>)"

#define TEMPLATE_NAMED_ASSIGNMENT "parameters[\'<paramname>\'] = <paramname>;"
#define TEMPLATE_POSITION_ASSIGNMENT "parameters.append(<paramname>);"

#define TEMPLATE_METHODCALL "result = self.callMethod(\'<name>\', parameters);"
#define TEMPLATE_NOTIFICATIONCALL "self.callNotification(\'<name>\', parameters);"

// #define TEMPLATE_RETURNCHECK "if (result<cast>)"
#define TEMPLATE_RETURN "return result<cast>;"

using namespace std;
using namespace jsonrpc;


PythonClientStubGenerator::PythonClientStubGenerator(const string &stubname, std::vector<Procedure> &procedures, std::ostream& outputstream) :
    StubGenerator(stubname, procedures, outputstream)
{
}

PythonClientStubGenerator::PythonClientStubGenerator(const string &stubname, std::vector<Procedure> &procedures, const string filename) :
    StubGenerator(stubname, procedures, filename)
{
}

void PythonClientStubGenerator::generateStub()
{
    // vector<string> classname = PythonHelper::splitPackages(this->stubname);
    // PythonHelper::prolog(*this, this->stubname);
    this->writeLine("#");
    this->writeLine("# This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!");
    this->writeLine("#");
    this->writeNewLine();
    this->writeLine("import client");
    this->writeNewLine();

    // int depth = PythonHelper::namespaceOpen(*this, stubname);

    // this->writeLine(replaceAll(TEMPLATE_PYTHON_CLIENT_SIGCLASS, "<stubname>", classname.at(classname.size()-1)));
    this->write(replaceAll(TEMPLATE_PYTHON_CLIENT_SIGCLASS, "<stubname>", this->stubname));
    this->writeLine(":");
    this->increaseIndentation();

    // this->writeLine(replaceAll(TEMPLATE_PYTHON_CLIENT_SIGCONSTRUCTOR, "<stubname>", classname.at(classname.size()-1)));
    this->writeLine(replaceAll(TEMPLATE_PYTHON_CLIENT_SIGCONSTRUCTOR, "<stubname>", this->stubname));
    this->writeNewLine();

    for (unsigned int i=0; i < procedures.size(); i++)
    {
        this->generateMethod(procedures[i]);
    }

    this->decreaseIndentation();
    this->writeNewLine();
}

void PythonClientStubGenerator::generateMethod(Procedure &proc)
{
    string procsignature = TEMPLATE_PYTHON_CLIENT_SIGMETHOD;
    // string returntype = PythonHelper::toCppReturntype(proc.GetReturnType());
    // if (proc.GetProcedureType() == RPC_NOTIFICATION)
        // returntype = "void";

    // replaceAll2(procsignature, "<returntype>", returntype);
    replaceAll2(procsignature, "<methodname>", proc.GetProcedureName());

    // generate parameters string
    string params = generateParameterDeclarationList(proc);
    replaceAll2(procsignature, "<parameters>", params);

    this->write(procsignature);
    this->writeLine(":");
    this->increaseIndentation();

    // this->writeLine("Json::Value p;");
    if (proc.GetParameterDeclarationType() == PARAMS_BY_NAME)
    {
        this->writeLine("parmeters = {}");
    }
    else if(proc.GetParameterDeclarationType() == PARAMS_BY_POSITION)
    {
        this->writeLine("parameters = []");
    }

    generateAssignments(proc);
    this->writeNewLine();
    generateProcCall(proc);
    this->writeNewLine();

    this->decreaseIndentation();


}

string PythonClientStubGenerator::generateParameterDeclarationList(Procedure &proc)
{
    stringstream param_string;
    parameterNameList_t list = proc.GetParameters();

    for (parameterNameList_t::iterator it = list.begin(); it != list.end();)
    {
        param_string << it->first;
        if(++it != list.end())
        {
            param_string << ", ";
        }
    }

    return param_string.str();
}

void PythonClientStubGenerator::generateAssignments(Procedure &proc)
{
    string assignment;
    parameterNameList_t list = proc.GetParameters();
    if(list.size() > 0)
    {
        for (parameterNameList_t::iterator it = list.begin(); it != list.end(); ++it)
        {

            if(proc.GetParameterDeclarationType() == PARAMS_BY_NAME)
            {
                assignment = TEMPLATE_NAMED_ASSIGNMENT;
            }
            else
            {
                assignment = TEMPLATE_POSITION_ASSIGNMENT;
            }
            replaceAll2(assignment, "<paramname>", it->first);
            this->writeLine(assignment);
        }
    }
    else
    {
        this->writeLine("parameters = None");
    }

}

void PythonClientStubGenerator::generateProcCall(Procedure &proc)
{
    string call;
    if (proc.GetProcedureType() == RPC_METHOD)
    {
        call = TEMPLATE_METHODCALL;
        this->writeLine(replaceAll(call, "<name>", proc.GetProcedureName()));
        // call = TEMPLATE_RETURNCHECK;
        // replaceAll2(call,"<cast>", PythonHelper::isCppConversion(proc.GetReturnType()));
        // this->writeLine(call);
        // this->increaseIndentation();
        // call = TEMPLATE_RETURN;
        // replaceAll2(call,"<cast>", PythonHelper::toCppConversion(proc.GetReturnType()));
        // this->writeLine(call);
        // this->decreaseIndentation();
        // this->writeLine("else");
        // this->increaseIndentation();
        // this->writeLine("throw jsonrpc::JsonRpcException(jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE, result.toStyledString());");
        // this->decreaseIndentation();
        this->writeLine("return result");
    }
    else
    {
        call = TEMPLATE_NOTIFICATIONCALL;
        replaceAll2(call, "<name>", proc.GetProcedureName());
        this->writeLine(call);
    }

}

string PythonClientStubGenerator::class2Filename(const string &classname)
{
    string result = classname;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result + ".py";
}
