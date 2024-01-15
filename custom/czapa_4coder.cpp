
#if !defined(FAODER_DEFAULT_BINDINGS_APP)
#define FAODER_DEFAULT_BINDINGS_APP

#include "4coder_default_include.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

fn BeginUndoRedoBarrier
(Application_Links* app)
{
    View_ID ViewId = get_active_view(app, Access_Always);
    Buffer_ID Buffer = view_get_buffer(app, ViewId, Access_Always);
    History_Group HistoryGroup = history_group_begin(app, Buffer);
    return HistoryGroup;
}

#define ScopeUndoRedoBarrier \
auto _HisGroup = BeginUndoRedoBarrier(app); \
defer(history_group_end(_HisGroup))

fn GetCursorPos
(Application_Links* app)
{
    auto View = get_active_view(app, Access_ReadVisible);
    return view_get_cursor_pos(app, View);
}

fn GetMarkPos
(Application_Links* app)
{
    auto View = get_active_view(app, Access_ReadVisible);
    return view_get_mark_pos(app, View);
}

struct cursor_and_mark_positions
{
    i64 CursorPos, MarkPos;
    Application_Links* app;
    View_ID View;
};

fn GetCursorAndMarkPositions
(Application_Links* app)
{
    cursor_and_mark_positions Res;
    Res.app = app;
    Res.View = get_active_view(Res.app, Access_ReadVisible);
    Res.CursorPos = view_get_cursor_pos(Res.app, Res.View);
    Res.MarkPos = view_get_mark_pos(Res.app, Res.View);
    return Res;
}

fn SetCursorAndMarkPositionsTo
(cursor_and_mark_positions Positions)
{
    view_set_cursor(Positions.app, Positions.View, seek_pos(Positions.CursorPos));
    view_set_mark(Positions.app, Positions.View, seek_pos(Positions.MarkPos));
}

struct temporary_cursor_and_mark_positions
{
    cursor_and_mark_positions Positions;
    
    temporary_cursor_and_mark_positions(Application_Links* App)
    { Positions = GetCursorAndMarkPositions(App); }
    
    ~temporary_cursor_and_mark_positions()
    { SetCursorAndMarkPositionsTo(Positions); }
};

#define ScopeTemporaryChangesToCursorAndMarkPos \
temporary_cursor_and_mark_positions _TemporaryCursorAndMarkPositions(app);

fn GetBeginningOfLinePos
(Application_Links* app, View_ID View, i64 PosSomewhereInsideLine)
{
    Buffer_Cursor Cursor = view_compute_cursor(app, View, seek_pos(PosSomewhereInsideLine));
    Vec2_f32 P = view_relative_xy_of_pos(app, View, Cursor.line, PosSomewhereInsideLine);
    P.x = 0;
    return view_pos_at_relative_xy(app, View, Cursor.line, P);
}

fn GetEndOfLinePos
(Application_Links* app, View_ID View, i64 PosSomewhereInsideLine)
{
    Buffer_Cursor Cursor = view_compute_cursor(app, View, seek_pos(PosSomewhereInsideLine));
    Vec2_f32 P = view_relative_xy_of_pos(app, View, Cursor.line, PosSomewhereInsideLine);
    P.x = max_f32;
    return view_pos_at_relative_xy(app, View, Cursor.line, P);
}

fn SetCurrentMapId
(Application_Links* App, Command_Map_ID MapId)
{
    View_ID View = get_active_view(App, 0);
    Buffer_ID Buffer = view_get_buffer(App, View, 0);
    Managed_Scope Scope = buffer_get_managed_scope(App, Buffer);
    Command_Map_ID* MapIdPtr = scope_attachment(App, Scope, buffer_map_id, Command_Map_ID);
    *MapIdPtr = MapId;
}

fn GetColorInt
(u8 R, u8 G, u8 B)
{
    union color
    {
        struct
        { u8 B, G, R, A; };
        
        u32 Int;
    };
    
    color C;
    C.R = R;
    C.G = G;
    C.B = B;
    C.A = 255;
    return C.Int;
}

const u32 White = GetColorInt(250, 250, 250);
const u32 Black = GetColorInt(0, 0, 0);

fn WriteText(Application_Links* App, const char* Str)
{ write_text(App, ToStringConstU8(Str)); }

template<u32 Size>
fn WriteText(Application_Links* App, static_string<Size>& Str)
{ write_text(App, ToStringConstU8(Str)); }

fn MoveHorizontally
(Application_Links* app, i32 CharacterCount)
{
    View_ID view = get_active_view(app, Access_ReadVisible);
    view_set_cursor_by_character_delta(app, view, CharacterCount);
    no_mark_snap_to_cursor_if_shift(app, view);
}

CUSTOM_COMMAND_SIG(SelectSurroundingScope)
{
    select_surrounding_scope(app);
    move_right(app);
    cursor_mark_swap(app);
    move_left(app);
    cursor_mark_swap(app);
}

CUSTOM_COMMAND_SIG(WriteTodo)
{ WriteText(app, " // TODO: "); }

CUSTOM_COMMAND_SIG(WriteTodoNow)
{ WriteText(app, " // TODO(now): "); }

CUSTOM_COMMAND_SIG(WriteTodoNowUseOrDeleteIt)
{ WriteText(app, " // TODO(now): Use or delete it"); }

CUSTOM_COMMAND_SIG(WriteNote)
{ WriteText(app, " // NOTE: "); }

CUSTOM_COMMAND_SIG(WriteReturn)
{ WriteText(app, "return "); }

CUSTOM_COMMAND_SIG(WriteReturnFromVoidFunction)
{ WriteText(app, "return;"); }

CUSTOM_COMMAND_SIG(WriteAuto)
{ WriteText(app, "auto "); }

CUSTOM_COMMAND_SIG(WriteAutoReference)
{ WriteText(app, "auto& "); }

CUSTOM_COMMAND_SIG(WriteConst)
{ WriteText(app, "const "); }

CUSTOM_COMMAND_SIG(WriteBool)
{ WriteText(app, "bool "); }

CUSTOM_COMMAND_SIG(WriteU32)
{ WriteText(app, "u32 "); }

CUSTOM_COMMAND_SIG(WriteF32)
{ WriteText(app, "f32 "); }

CUSTOM_COMMAND_SIG(WriteV2)
{ WriteText(app, "v2 "); }

CUSTOM_COMMAND_SIG(WriteArrowOperator)
{ WriteText(app, "->"); }

CUSTOM_COMMAND_SIG(OpenOneLineBraces)
{ 
    WriteText(app, "{}");
    move_left(app);
}

CUSTOM_COMMAND_SIG(OpenSingleBrace)
{ WriteText(app, "{"); }

CUSTOM_COMMAND_SIG(ChangeMultipleLineBracesIntoSingleLineBraces)
{ 
    ScopeUndoRedoBarrier;
    seek_beginning_of_line(app);
    backspace_char(app);
    WriteText(app, " ");
    move_down(app);
    seek_beginning_of_line(app);
    backspace_char(app);
    WriteText(app, " ");
    move_left(app);
}

CUSTOM_COMMAND_SIG(ChangeSingleLineBracesIntoMultipleLineBraces)
{ 
    ScopeUndoRedoBarrier;
    seek_beginning_of_line(app);
    move_right(app);
    delete_char(app);
    WriteText(app, "\n");
    seek_end_of_line(app);
    move_left(app);
    backspace_char(app);
    WriteText(app, "\n");
    move_up(app);
    seek_beginning_of_line(app);
}

CUSTOM_COMMAND_SIG(OpenParentheses)
{ 
    WriteText(app, "()");
    move_left(app);
}

CUSTOM_COMMAND_SIG(OpenParenthesesWithSemicolon)
{ 
    WriteText(app, "();");
    MoveHorizontally(app, -2);
}

CUSTOM_COMMAND_SIG(OpenSingleParenthesis)
{ WriteText(app, "("); }

CUSTOM_COMMAND_SIG(OpenQuotes)
{ 
    WriteText(app, "\"\"");
    move_left(app);
}

CUSTOM_COMMAND_SIG(OpenSingleQuote)
{ WriteText(app, "\""); }

CUSTOM_COMMAND_SIG(WriteIf)
{
    WriteText(app, "if()");
    move_left(app);
}

CUSTOM_COMMAND_SIG(WriteElse)
{ WriteText(app, "else\n"); }

CUSTOM_COMMAND_SIG(WriteElseIf)
{ 
    WriteText(app, "else if()");
    move_left(app);
}

CUSTOM_COMMAND_SIG(WriteStruct)
{ 
    ScopeUndoRedoBarrier;
    WriteText(app, "\nstruct ");
    set_mark(app);
    WriteText(app, "\n{\n\n};\n");
    move_vertical_lines(app, -2);
    cursor_mark_swap(app);
}

fn InternalCopyTypeNameToVarName
(Application_Links* app)
{
    Scratch_Block Scratch(app);
    set_mark(app);
    move_left_whitespace_or_token_boundary(app);
    cursor_mark_swap(app);
    View_ID View = get_active_view(app, 0);
    Buffer_ID Buffer = view_get_buffer(app, View, 0);
    Range_i64 Range = get_view_range(app, View);
    auto RangeString = push_buffer_range(app, Scratch, Buffer, Range);
    static_string<> VarNameString = {};
    VarNameString += ' ';
    bool BeginningOfWord = true;
    for(u32 I = 0; I < RangeString.size; I++)
    {
        char C = RangeString.str[I];
        if(C == '_')
        {
            BeginningOfWord = true;
        }
        else
        {
            if(BeginningOfWord)
            {
                VarNameString += (char)toupper(C);
                BeginningOfWord = false;
            }
            else
            {
                VarNameString += C;
            }
        }
    }
    WriteText(app, VarNameString);
}

CUSTOM_COMMAND_SIG(CopyTypeNameToVarName)
{ 
    ScopeUndoRedoBarrier;
    InternalCopyTypeNameToVarName(app);
}

CUSTOM_COMMAND_SIG(CopyTypeNameToVarNameWithReference)
{
    ScopeUndoRedoBarrier;
    InternalCopyTypeNameToVarName(app);
    move_left_alpha_numeric_boundary(app);
    move_left(app);
    WriteText(app, "&");
    move_right_alpha_numeric_boundary(app);
}

CUSTOM_COMMAND_SIG(CopyTypeNameToVarNameWithPointer)
{
    InternalCopyTypeNameToVarName(app);
    ScopeUndoRedoBarrier;
    move_left_alpha_numeric_boundary(app);
    move_left(app);
    WriteText(app, "*");
    move_right_alpha_numeric_boundary(app);
}

fn SetNormalModeColors()
{
    u32 Orange = GetColorInt(230, 100, 20);
    active_color_table.arrays[defcolor_cursor].vals[0] = Orange;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
    active_color_table.arrays[defcolor_margin_active].vals[0] = Orange;
}

fn ResetOpNumberState()
{
    CzapaState.OperationNumber.Number = 1;
    CzapaState.OperationNumber.Modified = false;
}

CUSTOM_COMMAND_SIG(GoToNormalMode)
{
    SetCurrentMapId(app, CzapaState.ModeMaps.Normal);
	SetNormalModeColors();
    ResetOpNumberState();
    save_all_dirty_buffers(app);
}

CUSTOM_COMMAND_SIG(GoToInsertMode)
{
    SetCurrentMapId(app, CzapaState.ModeMaps.Insert);
    
    u32 Green = GetColorInt(50, 230, 50);
    active_color_table.arrays[defcolor_cursor].vals[0] = Green;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
    active_color_table.arrays[defcolor_margin_active].vals[0] = Green;
}

CUSTOM_COMMAND_SIG(GoToDeleteMode)
{
    SetCurrentMapId(app, CzapaState.ModeMaps.Delete);
    
    u32 Red = GetColorInt(230, 50, 50);
    active_color_table.arrays[defcolor_cursor].vals[0] = Red;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
    
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(GoToChangeMode)
{
    SetCurrentMapId(app, CzapaState.ModeMaps.Change);
    
    u32 Yellow = GetColorInt(200, 200, 50);
    active_color_table.arrays[defcolor_cursor].vals[0] = Yellow;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = Black;
    
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(GoToPanelMode)
{
    SetCurrentMapId(app, CzapaState.ModeMaps.Panel);
    
    u32 Pink = GetColorInt(255, 105, 180);
    active_color_table.arrays[defcolor_cursor].vals[0] = Pink;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
}

CUSTOM_COMMAND_SIG(MoveUp)
{
    move_vertical_lines(app, -CzapaState.OperationNumber.Number);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveDown)
{
    move_vertical_lines(app, CzapaState.OperationNumber.Number);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveRight)
{ 
    MoveHorizontally(app, CzapaState.OperationNumber.Number); 
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLeft)
{ 
    MoveHorizontally(app, -CzapaState.OperationNumber.Number);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericBoundaryLeft)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_left_alpha_numeric_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericOrCamelBoundaryLeft)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_left_alpha_numeric_or_camel_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericBoundaryRight)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_right_alpha_numeric_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericOrCamelBoundaryRight)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_right_alpha_numeric_or_camel_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineUp)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_line_up(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineDown)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_line_down(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineUp10)
{
    for(u32 I = 0; I < 10; ++I)
        move_line_up(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineDown10)
{
    for(u32 I = 0; I < 10; ++I)
        move_line_down(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveUpToBlankLineEnd)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_up_to_blank_line_end(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveDownToBlankLineEnd)
{
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_down_to_blank_line_end(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(ChangeActivePanel)
{
	change_active_panel(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeActivePanelBackwards)
{
	change_active_panel_backwards(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(SwapPanels)
{
    swap_panels(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ViewBufferOtherPanel)
{
	view_buffer_other_panel(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(OpenPanelVSplit)
{
	open_panel_vsplit(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(OpenPanelHSplit)
{
	open_panel_hsplit(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ClosePanel)
{
	close_panel(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(SelectLine)
{
    seek_beginning_of_line(app);
    set_mark(app);
    seek_end_of_line(app);
}

CUSTOM_COMMAND_SIG(CopyLine)
{
    ScopeTemporaryChangesToCursorAndMarkPos;
    SelectLine(app);
    copy(app);
}

CUSTOM_COMMAND_SIG(DeleteWordRight)
{
    ScopeUndoRedoBarrier;
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        delete_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(CutWordRight)
{
    ScopeUndoRedoBarrier;
    auto CursorAndMarkPositions = GetCursorAndMarkPositions(app);
    set_mark(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_right_alpha_numeric_boundary(app);
    copy(app);
    SetCursorAndMarkPositionsTo(CursorAndMarkPositions);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        delete_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteWordLeft)
{
    ScopeUndoRedoBarrier;
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        backspace_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(CutWordLeft)
{
    ScopeUndoRedoBarrier;
    auto CursorAndMarkPositions = GetCursorAndMarkPositions(app);
    set_mark(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        move_left_alpha_numeric_boundary(app);
    copy(app);
    SetCursorAndMarkPositionsTo(CursorAndMarkPositions);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        backspace_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteLine)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(CutLine)
{
    ScopeUndoRedoBarrier;
    CopyLine(app);
    delete_line(app);
    GoToNormalMode(app);
}

fn InternalDeleteUp
(Application_Links* app)
{
    delete_line(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
    {
        move_up(app);
        delete_line(app);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteUp)
{
    ScopeUndoRedoBarrier;
    InternalDeleteUp(app);
}

CUSTOM_COMMAND_SIG(CutUp)
{
    ScopeUndoRedoBarrier;
    auto CursorAndMarkPositions = GetCursorAndMarkPositions(app);
    seek_end_of_line(app);
    set_mark(app);
    move_vertical_lines(app, -CzapaState.OperationNumber.Number);
    seek_beginning_of_line(app);
    copy(app);
    SetCursorAndMarkPositionsTo(CursorAndMarkPositions);
    InternalDeleteUp(app);
}

fn InternalDeleteDown
(Application_Links* app)
{
    delete_line(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        delete_line(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteDown)
{
    ScopeUndoRedoBarrier;
    InternalDeleteDown(app);
}

CUSTOM_COMMAND_SIG(CutDown)
{
    ScopeUndoRedoBarrier;
    auto CursorAndMarkPositions = GetCursorAndMarkPositions(app);
    seek_beginning_of_line(app);
    set_mark(app);
    move_vertical_lines(app, CzapaState.OperationNumber.Number);
    seek_end_of_line(app);
    copy(app);
    SetCursorAndMarkPositionsTo(CursorAndMarkPositions);
    InternalDeleteDown(app);
}

CUSTOM_COMMAND_SIG(ChangeLine)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    WriteText(app, "\n");
    GoToInsertMode(app);
    move_up(app);
}

CUSTOM_COMMAND_SIG(ChangeWordRight)
{
    ScopeUndoRedoBarrier;
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        delete_alpha_numeric_boundary(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ChangeWordLeft)
{
    ScopeUndoRedoBarrier;
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        backspace_alpha_numeric_boundary(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ChangeUp)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
    {
        move_up(app);
        delete_line(app);
    }
    WriteText(app, "\n");
    move_up(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ChangeDown)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(i32 I = 0; I < CzapaState.OperationNumber.Number; ++I)
        delete_line(app);
    WriteText(app, "\n");
    move_up(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(NewLine)
{
    seek_end_of_line(app);
    WriteText(app, "\n");
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(NewLineStayingAtTheSamePlace)
{
    seek_end_of_line(app);
    WriteText(app, "\n");
    move_up(app);
}

CUSTOM_COMMAND_SIG(MoveRightAndGoToInsertMode)
{
    move_right(app);
    GoToInsertMode(app);
}

fn InputNumber
(i32 N)
{
    auto& OperationNumber = CzapaState.OperationNumber;
    if(OperationNumber.Modified)
    {
        OperationNumber.Number *= 10;
        OperationNumber.Number += N;
    }
    else if(N != 0)
    {
        OperationNumber.Number = N;
        OperationNumber.Modified = true;
    }
}

CUSTOM_COMMAND_SIG(InputNumber0)
{ InputNumber(0); }

CUSTOM_COMMAND_SIG(InputNumber1)
{ InputNumber(1); }

CUSTOM_COMMAND_SIG(InputNumber2)
{ InputNumber(2); }

CUSTOM_COMMAND_SIG(InputNumber3)
{ InputNumber(3); }

CUSTOM_COMMAND_SIG(InputNumber4)
{ InputNumber(4); }

CUSTOM_COMMAND_SIG(InputNumber5)
{ InputNumber(5); }

CUSTOM_COMMAND_SIG(InputNumber6)
{ InputNumber(6); }

CUSTOM_COMMAND_SIG(InputNumber7)
{ InputNumber(7); }

CUSTOM_COMMAND_SIG(InputNumber8)
{ InputNumber(8); }

CUSTOM_COMMAND_SIG(InputNumber9)
{ InputNumber(9); }

fn BindInputNumbers
(Mapping* m, Command_Map* map)
{
    Bind(InputNumber0, KeyCode_0);
    Bind(InputNumber1, KeyCode_1);
    Bind(InputNumber2, KeyCode_2);
    Bind(InputNumber3, KeyCode_3);
    Bind(InputNumber4, KeyCode_4);
    Bind(InputNumber5, KeyCode_5);
    Bind(InputNumber6, KeyCode_6);
    Bind(InputNumber7, KeyCode_7);
    Bind(InputNumber8, KeyCode_8);
    Bind(InputNumber9, KeyCode_9);
}

CUSTOM_COMMAND_SIG(SearchRangeBetweenCursorAndMarker)
{
    View_ID View = get_active_view(app, Access_ReadWriteVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadWriteVisible);
    Range_i64 Range = get_view_range(app, View);
    Scratch_Block Scratch(app);
    String_Const_u8 SearchedText = push_buffer_range(app, Scratch, Buffer, Range);
    isearch(app, Scan_Forward, Range.first, SearchedText);
    
}
CUSTOM_COMMAND_SIG(SearchUsingPreviousValue)
{
    View_ID View = get_active_view(app, Access_ReadWriteVisible);
    i64 CursorPos = view_get_cursor_pos(app, View);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadWriteVisible);
    i64 BufferSize = buffer_get_size(app, Buffer);
    String_Const_u8 SearchedText = ToStringConstU8(CzapaState.LastSearch);
    i64 NewPos;
    seek_string_insensitive_forward(app, Buffer, CursorPos, BufferSize, SearchedText, &NewPos);
    if(NewPos < BufferSize)
        isearch(app, Scan_Forward, NewPos, SearchedText);
    else
        isearch(app, Scan_Forward, CursorPos, SearchedText);
}

CUSTOM_COMMAND_SIG(DeleteUntilEndOfLine)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    i64 CurrentPos = view_get_cursor_pos(app, View);
    i64 EndOfLinePos = GetEndOfLinePos(app, View, CurrentPos);
    Range_i64 Range = {CurrentPos, EndOfLinePos};
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadWriteVisible);
    if(clipboard_post_buffer_range(app, 0, Buffer, Range))
        buffer_replace_range(app, Buffer, Range, string_u8_empty);
}

CUSTOM_COMMAND_SIG(ChangeUntilEndOfLine)
{
    DeleteUntilEndOfLine(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(DeleteUntilBeginningOfLine)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    i64 CurrentPos = view_get_cursor_pos(app, View);
    i64 BeginningOfLinePos = GetBeginningOfLinePos(app, View, CurrentPos);
    Range_i64 Range = {BeginningOfLinePos, CurrentPos};
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadWriteVisible);
    if(clipboard_post_buffer_range(app, 0, Buffer, Range))
        buffer_replace_range(app, Buffer, Range, string_u8_empty);
}

CUSTOM_COMMAND_SIG(ChangeUntilBeginningOfLine)
{
    DeleteUntilBeginningOfLine(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(DeleteInsideParens)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    i64 Pos = view_get_cursor_pos(app, View);
    Range_i64 Range = {};
    if(find_surrounding_nest(app, Buffer, Pos, FindNest_Paren, &Range))
    {
        Range.min++;
        Range.max--;
        buffer_replace_range(app, Buffer, Range, string_u8_empty);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeInsideParens)
{
    DeleteInsideParens(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(CopyInsideParens)
{
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    i64 Pos = view_get_cursor_pos(app, View);
    Range_i64 Range = {};
    if(find_surrounding_nest(app, Buffer, Pos, FindNest_Paren, &Range))
    {
        Range.min++;
        Range.max--;
        clipboard_post_buffer_range(app, 0, Buffer, Range);
    }
}

CUSTOM_COMMAND_SIG(CutInsideParens)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    i64 Pos = view_get_cursor_pos(app, View);
    Range_i64 Range = {};
    if(find_surrounding_nest(app, Buffer, Pos, FindNest_Paren, &Range))
    {
        Range.min++;
        Range.max--;
        if(clipboard_post_buffer_range(app, 0, Buffer, Range))
            buffer_replace_range(app, Buffer, Range, string_u8_empty);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteInsideScope)
{
    ScopeUndoRedoBarrier;
    SelectSurroundingScope(app);
    move_down(app);
    cursor_mark_swap(app);
    move_up(app);
    seek_end_of_line(app);
    Delete(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeInsideScope)
{
    DeleteInsideScope(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(CutInsideScope)
{
    ScopeUndoRedoBarrier;
    SelectSurroundingScope(app);
    move_down(app);
    cursor_mark_swap(app);
    move_up(app);
    seek_end_of_line(app);
    cut(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteInsideScope2)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    i64 Pos = view_get_cursor_pos(app, View);
    Range_i64 Range = {};
    if(find_surrounding_nest(app, Buffer, Pos, FindNest_Scope, &Range))
    {
        Range.min += 2;
        Range.max -= 2;
        buffer_replace_range(app, Buffer, Range, string_u8_empty);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeInsideScope2)
{
    DeleteInsideScope2(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(CutInsideScope2)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    i64 Pos = view_get_cursor_pos(app, View);
    Range_i64 Range = {};
    if(find_surrounding_nest(app, Buffer, Pos, FindNest_Scope, &Range))
    {
        Range.min += 2;
        Range.max -= 2;
        if(clipboard_post_buffer_range(app, 0, Buffer, Range))
            buffer_replace_range(app, Buffer, Range, string_u8_empty);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(DeleteBracesOfScope)
{
    ScopeUndoRedoBarrier;
    select_surrounding_scope(app);
    delete_current_scope(app);
    MoveDownToBlankLineEnd(app);
    delete_line(app);
    MoveUpToBlankLineEnd(app);
    delete_line(app);
    GoToNormalMode(app);
}

fn InternalDeleteLinesInsideRange
(Application_Links* app)
{
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    Range_i64 Range = get_view_range(app, View);
    Range.start = GetBeginningOfLinePos(app, View, Range.start);
    Range.end = GetEndOfLinePos(app, View, Range.end);
    buffer_replace_range(app, Buffer, Range, string_u8_empty);
}

CUSTOM_COMMAND_SIG(DeleteLinesInsideRange)
{
    ScopeUndoRedoBarrier;
    InternalDeleteLinesInsideRange(app);
    delete_line(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeLinesInsideRange)
{
    ScopeUndoRedoBarrier;
    InternalDeleteLinesInsideRange(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(CutLinesInsideRange)
{
    ScopeUndoRedoBarrier;
    View_ID View = get_active_view(app, Access_ReadVisible);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_ReadVisible);
    Range_i64 Range = get_view_range(app, View);
    Range.start = GetBeginningOfLinePos(app, View, Range.start);
    Range.end = GetEndOfLinePos(app, View, Range.end);
    if(clipboard_post_buffer_range(app, 0, Buffer, Range))
        buffer_replace_range(app, Buffer, Range, string_u8_empty);
    delete_line(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(PutLineInsideBraces)
{
    ScopeUndoRedoBarrier;
    seek_beginning_of_line(app);
    WriteText(app, "{\n");
    seek_end_of_line(app);
    WriteText(app, "\n}");
    move_up(app);
    seek_end_of_line(app);
}

CUSTOM_COMMAND_SIG(NewLineAndPasteAndIndent)
{
    ScopeUndoRedoBarrier;
    seek_end_of_line(app);
    WriteText(app, "\n");
    paste_and_indent(app);
}

CUSTOM_COMMAND_SIG(SetInvisibleMark)
{
    CzapaState.InvisibleMarkPos = GetCursorPos(app);
}

CUSTOM_COMMAND_SIG(SetCursorToInvisibleMark)
{
    auto View = get_active_view(app, Access_ReadVisible);
    view_set_cursor(app, View, seek_pos(CzapaState.InvisibleMarkPos));
    center_view(app);
}

CUSTOM_COMMAND_SIG(SearchTodoNow)
{
    auto String = ToStringConstU8("todo(now)");
    list_all_locations__generic(app, String, 0);
}

CUSTOM_COMMAND_SIG(ReopenAllLoadedFiles)
{
    Scratch_Block scratch(app);
    for (Buffer_ID buffer = get_buffer_next(app, 0, Access_ReadWriteVisible);
         buffer != 0;
         buffer = get_buffer_next(app, buffer, Access_ReadWriteVisible)){
        Dirty_State dirty = buffer_get_dirty_state(app, buffer);
        if (dirty != DirtyState_UnsavedChanges){
            buffer_reopen(app, buffer, 0);
        }
    }
}

CUSTOM_COMMAND_SIG(CreateBreakpointInRemedyBG)
{
    Scratch_Block Scratch(app);
    View_ID View = get_active_view(app, Access_Always);
    Buffer_ID Buffer = view_get_buffer(app, View, Access_Always);
    String_Const_u8 FileName = push_buffer_file_name(app, Scratch, Buffer);
    i64 CursorPos = view_get_cursor_pos(app, View);
    Buffer_Cursor Cursor = view_compute_cursor(app, View, seek_pos(CursorPos));
    
    char Command[512];
    sprintf(Command, "start remedybg add-breakpoint-at-file %s %d", GetCString(FileName), (i32)Cursor.line);
    system(Command);
}

fn SetupSharedKeyBindings
(Mapping* m, Command_Map* map)
{
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
    BindTextInput(write_text_input);
    
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    
    BindMouseMove(click_set_cursor_if_lbutton);
    Bind(interactive_new,               KeyCode_N, KeyCode_Control);
    Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
    Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
    Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
    Bind(close_build_panel,             KeyCode_Comma, KeyCode_Alt);
    Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
    Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
    Bind(build_in_build_panel,          KeyCode_M, KeyCode_Alt);
    Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
    Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
    Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
    Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Alt);
    Bind(jump_to_last_point,            KeyCode_P, KeyCode_Control);
    Bind(list_all_functions_current_buffer_lister, KeyCode_I, KeyCode_Control, KeyCode_Shift);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(page_up,                KeyCode_PageUp);
    Bind(page_down,              KeyCode_PageDown);
    Bind(toggle_fullscreen, KeyCode_F11);
}

fn SetupKeyBindings
(Mapping *mapping)
{
    auto& ModeMaps = CzapaState.ModeMaps;
    ModeMaps.Normal = vars_save_string_lit("mapid_normal");
    ModeMaps.Insert = vars_save_string_lit("mapid_insert");
    ModeMaps.Delete = vars_save_string_lit("mapid_delete");
    ModeMaps.Change = vars_save_string_lit("mapid_change");
    ModeMaps.Panel = vars_save_string_lit("mapid_panel");
    
    MappingScope();
    SelectMapping(mapping);
    
    // normal mode
    SelectMap(ModeMaps.Normal);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    
    Bind(save_all_dirty_buffers, KeyCode_Escape);
    Bind(reopen, KeyCode_F2);
    Bind(ReopenAllLoadedFiles, KeyCode_F2, KeyCode_Alt);
    Bind(list_all_substring_locations_case_insensitive, KeyCode_F3);
    Bind(list_all_locations_of_identifier, KeyCode_F3, KeyCode_Control);
    Bind(list_all_substring_locations_of_selection, KeyCode_F3, KeyCode_Alt);
    Bind(replace_in_buffer, KeyCode_F4);
    Bind(replace_in_all_buffers, KeyCode_F4, KeyCode_Shift);
    Bind(ReplaceInBufferIdentifier, KeyCode_F4, KeyCode_Control);
    Bind(ReplaceInAllBuffersIdentifier, KeyCode_F4, KeyCode_Shift, KeyCode_Control);
    Bind(ReplaceInBufferSelection, KeyCode_F4, KeyCode_Control, KeyCode_Alt);
    Bind(ReplaceInAllBuffersSelection, KeyCode_F4, KeyCode_Shift, KeyCode_Control, KeyCode_Alt);
    Bind(query_replace, KeyCode_F5);
    Bind(query_replace_identifier, KeyCode_F5, KeyCode_Control);
    Bind(query_replace_selection, KeyCode_F5, KeyCode_Alt);
    Bind(QueryReplaceUsingPreviousReplaceValue, KeyCode_F5, KeyCode_Shift);
    Bind(QueryReplaceUsingPreviousValues, KeyCode_F5, KeyCode_Shift, KeyCode_Alt);
    Bind(jump_to_definition, KeyCode_F6);
    Bind(jump_to_definition_at_cursor, KeyCode_F6, KeyCode_Control);
    Bind(JumpToDefinitionInOtherPanel, KeyCode_F6, KeyCode_Control, KeyCode_Shift);
    Bind(CreateBreakpointInRemedyBG, KeyCode_F7);
    Bind(SearchTodoNow, KeyCode_F8);
    Bind(open_matching_file_cpp, KeyCode_F9);
    Bind(OpenMatchingFileCppInCurrentView, KeyCode_F9, KeyCode_Control);
    Bind(toggle_line_numbers, KeyCode_F10);
    Bind(load_project, KeyCode_F12);
    Bind(place_in_scope, KeyCode_Equal);
    Bind(backspace_char, KeyCode_Backspace);
    Bind(delete_char, KeyCode_Delete);
    Bind(center_view, KeyCode_Minus);
    
    Bind(cursor_mark_swap, KeyCode_Q);
    Bind(SetCursorToInvisibleMark, KeyCode_Q, KeyCode_Shift);
    Bind(MoveAlphaNumbericBoundaryRight, KeyCode_W);
    Bind(MoveAlphaNumbericOrCamelBoundaryRight, KeyCode_W, KeyCode_Control);
    Bind(SelectSurroundingScope, KeyCode_E);
    Bind(redo, KeyCode_R);
    Bind(duplicate_line, KeyCode_T);
    Bind(copy, KeyCode_Y);
    Bind(CopyLine, KeyCode_Y, KeyCode_Control);
    Bind(CopyInsideParens, KeyCode_Y, KeyCode_Alt);
    Bind(undo, KeyCode_U);
    Bind(GoToInsertMode, KeyCode_I);
    Bind(command_lister, KeyCode_I, KeyCode_Control);
    Bind(NewLine, KeyCode_O);
    Bind(NewLineStayingAtTheSamePlace, KeyCode_O, KeyCode_Alt);
    Bind(interactive_open_or_new, KeyCode_O, KeyCode_Control);
    Bind(interactive_switch_buffer, KeyCode_O, KeyCode_Control, KeyCode_Shift);
    Bind(paste_and_indent, KeyCode_P);
    Bind(NewLineAndPasteAndIndent, KeyCode_P, KeyCode_Alt);
    Bind(open_in_other, KeyCode_P, KeyCode_Control);
    Bind(MoveUpToBlankLineEnd, KeyCode_LeftBracket);
    Bind(MoveDownToBlankLineEnd, KeyCode_RightBracket);
    Bind(PutLineInsideBraces, KeyCode_LeftBracket, KeyCode_Shift);
    Bind(ChangeMultipleLineBracesIntoSingleLineBraces, KeyCode_LeftBracket, KeyCode_Alt);
    Bind(ChangeSingleLineBracesIntoMultipleLineBraces, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Control);
    Bind(snippet_lister, KeyCode_Period);
    
    Bind(MoveRightAndGoToInsertMode, KeyCode_A);
    Bind(delete_char, KeyCode_S);
    Bind(GoToDeleteMode, KeyCode_D);
    Bind(DeleteUntilEndOfLine, KeyCode_D, KeyCode_Shift);
    Bind(DeleteUntilBeginningOfLine, KeyCode_D, KeyCode_Alt);
    Bind(SelectLine, KeyCode_F);
    Bind(goto_line, KeyCode_G);
    Bind(MoveLeft, KeyCode_H);
    Bind(seek_beginning_of_line, KeyCode_H, KeyCode_Control);
    Bind(MoveDown, KeyCode_J);
    Bind(move_down_10, KeyCode_J, KeyCode_Control);
    Bind(move_down_20, KeyCode_J, KeyCode_Shift);
    Bind(MoveLineDown, KeyCode_J, KeyCode_Alt);
    Bind(MoveLineDown10, KeyCode_J, KeyCode_Alt, KeyCode_Control);
    Bind(MoveUp, KeyCode_K);
    Bind(move_up_10, KeyCode_K, KeyCode_Control);
    Bind(move_up_20, KeyCode_K, KeyCode_Shift);
    Bind(MoveLineUp, KeyCode_K, KeyCode_Alt);
    Bind(MoveLineUp10, KeyCode_K, KeyCode_Alt, KeyCode_Control);
    Bind(MoveRight, KeyCode_L);
    Bind(seek_end_of_line, KeyCode_L, KeyCode_Control);
    Bind(comment_line_toggle, KeyCode_Semicolon);
    Bind(goto_jump_at_cursor, KeyCode_Return, KeyCode_Control);
    Bind(goto_jump_at_cursor_same_panel, KeyCode_Return, KeyCode_Alt);
    
    Bind(keyboard_macro_start_recording , KeyCode_Z, KeyCode_Shift);
    Bind(keyboard_macro_finish_recording, KeyCode_Z, KeyCode_Control);
    Bind(keyboard_macro_replay, KeyCode_Z);
    Bind(cut, KeyCode_X);
    Bind(Delete, KeyCode_X, KeyCode_Shift);
    Bind(GoToChangeMode, KeyCode_C);
    Bind(ChangeUntilEndOfLine, KeyCode_C, KeyCode_Shift);
    Bind(ChangeUntilBeginningOfLine, KeyCode_C, KeyCode_Alt);
    Bind(GoToPanelMode, KeyCode_V);
    Bind(MoveAlphaNumbericBoundaryLeft, KeyCode_B);
    Bind(MoveAlphaNumbericOrCamelBoundaryLeft, KeyCode_B, KeyCode_Control);
    Bind(write_block, KeyCode_B, KeyCode_Alt);
    Bind(set_mark, KeyCode_Space);
    Bind(set_mark, KeyCode_Space, KeyCode_Control);
    Bind(SetInvisibleMark, KeyCode_Space, KeyCode_Shift);
    Bind(goto_beginning_of_file, KeyCode_M);
    Bind(goto_end_of_file, KeyCode_N);
    Bind(ChangeActivePanel, KeyCode_Comma);
    //Bind(ChangeActivePanelBackwards, KeyCode_Comma, KeyCode_Control);
    Bind(search, KeyCode_ForwardSlash);
    Bind(search_identifier, KeyCode_ForwardSlash, KeyCode_Control);
    Bind(SearchRangeBetweenCursorAndMarker, KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(SearchUsingPreviousValue, KeyCode_ForwardSlash, KeyCode_Shift);
    
    // insert mode
    SelectMap(ModeMaps.Insert);
    SetupSharedKeyBindings(m, map);
    BindTextInput(write_text_and_auto_indent);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(move_left, KeyCode_Left);
    Bind(move_right, KeyCode_Right);
    Bind(move_up, KeyCode_Up);
    Bind(move_down, KeyCode_Down);
    Bind(ReopenAllLoadedFiles, KeyCode_F2, KeyCode_Alt);
    Bind(delete_char, KeyCode_Delete);
    Bind(backspace_char, KeyCode_Backspace);
    Bind(MoveUpToBlankLineEnd, KeyCode_Up, KeyCode_Control);
    Bind(MoveDownToBlankLineEnd, KeyCode_Down, KeyCode_Control);
    Bind(MoveAlphaNumbericBoundaryLeft,      KeyCode_Left, KeyCode_Control);
    Bind(MoveAlphaNumbericBoundaryRight,          KeyCode_Right, KeyCode_Control);
    Bind(MoveAlphaNumbericOrCamelBoundaryLeft,  KeyCode_Left, KeyCode_Alt);
    Bind(MoveAlphaNumbericOrCamelBoundaryRight, KeyCode_Right, KeyCode_Alt);
    Bind(NewLine, KeyCode_O, KeyCode_Alt);
    Bind(word_complete, KeyCode_Tab);
    Bind(word_complete_drop_down, KeyCode_Tab, KeyCode_Control);
    Bind(WriteArrowOperator, KeyCode_Minus, KeyCode_Alt);
    Bind(WriteReturn, KeyCode_R, KeyCode_Alt);
    Bind(WriteReturnFromVoidFunction, KeyCode_R, KeyCode_Alt, KeyCode_Shift);
    Bind(WriteTodo, KeyCode_T, KeyCode_Alt);
    Bind(WriteTodoNow, KeyCode_T, KeyCode_Alt, KeyCode_Shift);
    Bind(WriteTodoNowUseOrDeleteIt, KeyCode_T, KeyCode_Alt, KeyCode_Shift, KeyCode_Control);
    Bind(WriteNote, KeyCode_N, KeyCode_Alt);
    Bind(WriteBool, KeyCode_B, KeyCode_Alt);
    Bind(WriteU32, KeyCode_U, KeyCode_Alt);
    Bind(WriteF32, KeyCode_F, KeyCode_Alt);
    Bind(WriteV2, KeyCode_V, KeyCode_Alt);
    Bind(WriteIf, KeyCode_I, KeyCode_Alt);
    Bind(WriteElse, KeyCode_E, KeyCode_Alt);
    Bind(WriteElseIf, KeyCode_E, KeyCode_Alt, KeyCode_Shift);
    Bind(WriteAuto, KeyCode_A, KeyCode_Alt);
    Bind(WriteAutoReference, KeyCode_A, KeyCode_Alt, KeyCode_Shift);
    Bind(WriteStruct, KeyCode_S, KeyCode_Alt);
    Bind(CopyTypeNameToVarName, KeyCode_Y, KeyCode_Alt);
    Bind(CopyTypeNameToVarNameWithReference, KeyCode_Y, KeyCode_Alt, KeyCode_Shift);
    Bind(CopyTypeNameToVarNameWithPointer, KeyCode_Y, KeyCode_Alt, KeyCode_Control);
    Bind(OpenParentheses, KeyCode_9, KeyCode_Shift);
    Bind(OpenParenthesesWithSemicolon, KeyCode_9, KeyCode_Shift, KeyCode_Control);
    Bind(OpenSingleParenthesis, KeyCode_9, KeyCode_Shift, KeyCode_Alt);
    Bind(OpenQuotes, KeyCode_Quote, KeyCode_Shift);
    Bind(OpenSingleQuote, KeyCode_Quote, KeyCode_Shift, KeyCode_Alt);
    Bind(WriteConst, KeyCode_C, KeyCode_Alt);
    Bind(write_block, KeyCode_ForwardSlash, KeyCode_Alt);
    Bind(write_zero_struct, KeyCode_0, KeyCode_Alt);
    Bind(open_long_braces, KeyCode_LeftBracket, KeyCode_Shift);
    Bind(OpenOneLineBraces, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(OpenSingleBrace, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
    Bind(goto_end_of_file, KeyCode_PageDown, KeyCode_Control);
    Bind(seek_beginning_of_line, KeyCode_Home);
    Bind(seek_end_of_line, KeyCode_End);
    
    // delete mode
    SelectMap(ModeMaps.Delete);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    Bind(GoToNormalMode, KeyCode_Escape);
    //
    Bind(CutDown, KeyCode_J);
    Bind(DeleteDown, KeyCode_J, KeyCode_Control);
    //
    Bind(CutUp, KeyCode_K);
    Bind(DeleteUp, KeyCode_K, KeyCode_Control);
    //
    Bind(CutWordLeft, KeyCode_B);
    Bind(DeleteWordLeft, KeyCode_B, KeyCode_Control);
    //
    Bind(CutWordRight, KeyCode_W);
    Bind(DeleteWordRight, KeyCode_W, KeyCode_Control);
    //
    Bind(CutLine, KeyCode_D);
    Bind(DeleteLine, KeyCode_D, KeyCode_Control);
    //
    Bind(CutInsideParens, KeyCode_9);
    Bind(DeleteInsideParens, KeyCode_9, KeyCode_Control);
    //
    Bind(CutInsideScope, KeyCode_S);
    Bind(DeleteInsideScope, KeyCode_S, KeyCode_Control);
    Bind(CutInsideScope2, KeyCode_S, KeyCode_Alt);
    Bind(DeleteInsideScope2, KeyCode_S, KeyCode_Control, KeyCode_Alt);
    //
    Bind(CutLinesInsideRange, KeyCode_Q);
    Bind(DeleteLinesInsideRange, KeyCode_Q, KeyCode_Control);
    //
    Bind(DeleteBracesOfScope, KeyCode_LeftBracket);
    
    // change mode
    SelectMap(ModeMaps.Change);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(ChangeLine, KeyCode_C);
    Bind(ChangeWordRight, KeyCode_W);
    Bind(ChangeWordLeft, KeyCode_B);
    Bind(ChangeUp, KeyCode_K);
    Bind(ChangeDown, KeyCode_J);
    Bind(ChangeInsideParens, KeyCode_9);
    Bind(ChangeInsideScope, KeyCode_S);
    Bind(ChangeInsideScope2, KeyCode_S, KeyCode_Alt);
    Bind(ChangeLinesInsideRange, KeyCode_Q);
    
    // panel mode
    SelectMap(ModeMaps.Panel);
    SetupSharedKeyBindings(m, map);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(ChangeActivePanel, KeyCode_V);
    Bind(ChangeActivePanelBackwards, KeyCode_H);
    Bind(SwapPanels, KeyCode_S);
    Bind(ViewBufferOtherPanel, KeyCode_D);
    Bind(OpenPanelVSplit, KeyCode_V);
    Bind(OpenPanelHSplit, KeyCode_Minus);
    Bind(ClosePanel, KeyCode_0);
    
    // make default maps inherit mode maps
    String_ID GlobalMapId = vars_save_string_lit("keys_global");
    String_ID FileMapId = vars_save_string_lit("keys_file");
    String_ID CodeMapId  = vars_save_string_lit("keys_code");
    
    SelectMap(GlobalMapId);
    SetupSharedKeyBindings(m, map);
    
    SelectMap(FileMapId);
    ParentMap(ModeMaps.Normal);
    
    SelectMap(CodeMapId);
    ParentMap(ModeMaps.Normal);
}

void
custom_layer_init
(Application_Links *App)
{
    Thread_Context* tctx = get_thread_context(App);
    
    default_framework_init(App);
    set_all_default_hooks(App);
    mapping_init(tctx, &framework_mapping);
    
    SetupKeyBindings(&framework_mapping);  
    SetNormalModeColors();
    
    toggle_fullscreen(App);
}

#endif

/* TODO:
Changing variable name to my current style shortcut
Take in parenthesis shortcut - Put opening one after the cursor, Put closing one before the last character in the line (presumebly the last character is ;)
Make editor check if the file it has loaded has changed and then automatically load it again
--
(I asked qustion about it on the forum) Make the compilation shortcut open the build buffer under both panels
Make namespaces not indent
Make declarations inside namespace be indexed (so I can jump to them)

Add lister for putting code bookmarks so you can jump around
Modify the indenter
Saving on switching to normal mode, Need to have possibility of turning that off
*/

