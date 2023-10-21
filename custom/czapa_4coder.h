#include "czapa_base.h"

const i32 BuildBufferHeightInLines = 12;

struct czapa_state
{
    struct mode_maps
    {
        Command_Map_ID Insert;
        Command_Map_ID Normal;
        Command_Map_ID Delete;
        Command_Map_ID Change;
        Command_Map_ID Panel;
    };
    
    struct operation_number
    {
        bool Modified;
        i32 Number = 1;
    };
    
    struct last_replace
    { static_string<> ReplaceString, WithString; };
    
    mode_maps ModeMaps;
    operation_number OperationNumber;
    last_replace LastReplace;
    static_string<> LastSearch;
    i64 InvisibleMarkPos;
};
static czapa_state CzapaState;

static Snippet default_snippets[] = {
    {"fn", "\nfn \n()\n{\n\n}\n", 4, 6},
    {"value", "ImGui::Value(NameAndValue());", 26, 0},
    {"slider_float", "ImGui::SliderFloat(NameAndPtr());", 30, 32},
    {"fnvoid", "\nvoid \n()\n{\n\n}\n", 7, 13},
    {"fnvoid_one_line_header", "\nvoid ()\n{\n\n}\n", 7, 12},
    {"fn_custom_command_sig", "\nCUSTOM_COMMAND_SIG()\n{\n\n}\n", 20, 24},
    {"for", "for()\n{\n\n}", 4, 8},
    {"fornobraces", "for()\n", 4, 6},
    {"for;", "for(;;)\n{\n\n}", 4, 10},
    {"forentities", "ForEntitiesIn\n(.EntityGroup, [&]\n(entity& E)\n{\n\n});", 15, 47},
    {"forentities_type", "ForEntitiesIn\n(entity_type::, .EntityGroup, [&]\n(entity& E)\n{\n\n});", 28, 62},
    {"switch_case_with_braces", "switch()\n{\ncase :\n{\n\n} break;\n}", 7, 16},
    {"switch_case", "switch()\n{\ncase : break;\n}", 7, 16},
    {"switch", "switch()\n{\n\n}", 7, 11},
    {"enum_class", "enum class \n{\n\n};", 11, 14},
    {"enum", "enum \n{\n\n};", 5, 8},
    {"lambda_named_multiline", "\nauto  = [&]\n()\n{\n\n};\n", 6, 18},
    {"lambda_named_multiline no arguments", "\nauto  = [&]()\n{\n\n};\n", 6, 17},
    {"lambda_named_2lines", "\nauto  = [&]()\n{  };\n", 6, 16},
    {"lambda_named", "auto  = [&](){  };", 5, 14},
    {"lambda", "[](){  }", 6, 1},
    {"assert", "Assert();", 7, 0},
    {"v0", "v2(0.f)", 7, 0},
    {"v1", "v2(1.f)", 7, 0},
};
