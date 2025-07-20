#include "ScriptPane.h"

//(*InternalHeaders(ScriptPane)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ScriptPane)
const wxWindowID ScriptPane::ID_STATICTEXT1 = wxNewId();
const wxWindowID ScriptPane::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ScriptPane,wxPanel)
    //(*EventTable(ScriptPane)
    //*)
END_EVENT_TABLE()

ScriptPane::ScriptPane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(ScriptPane)
    wxBoxSizer* textInputLayout;
    wxFlexGridSizer* FlexGridSizer1;
    wxGridBagSizer* GridBagSizer1;

    Create(parent, wxID_ANY, wxDefaultPosition, wxSize(400,600), wxTAB_TRAVERSAL, _T("wxID_ANY"));
    FlexGridSizer1 = new wxFlexGridSizer(99, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    GridBagSizer1 = new wxGridBagSizer(0, 0);
    fileName = new wxStaticText(this, ID_STATICTEXT1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    GridBagSizer1->Add(fileName, wxGBPosition(0, 0), wxDefaultSpan, wxALL, 5);
    saveButton = new wxButton(this, ID_BUTTON2, _("Save"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    GridBagSizer1->Add(saveButton, wxGBPosition(0, 1), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    GridBagSizer1->AddGrowableCol(0);
    FlexGridSizer1->Add(GridBagSizer1, 1, wxALL|wxEXPAND, 5);
    textInputLayout = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(textInputLayout, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    Layout();
    //*)

    //scriptManager = new kScriptManager();

    editor = new wxStyledTextCtrl(this, wxID_ANY);
    textInputLayout->Add(editor, 1, wxEXPAND | wxALL, 0);

    editor->SetLexer(wxSTC_LEX_CPP);

    // Set keywords (you can define your AngelScript keywords here)
    editor->SetKeyWords(0, "and \
                            abstract* \
                            auto \
                            bool \
                            break \
                            case \
                            cast \
                            catch \
                            class \
                            const \
                            continue \
                            default \
                            delete* \
                            do \
                            double \
                            else \
                            enum \
                            explicit* \
                            external* \
                            false \
                            final* \
                            float \
                            for \
                            from* \
                            funcdef \
                            function* \
                            get* \
                            if \
                            import \
                            in \
                            inout \
                            int \
                            interface \
                            int8 \
                            int16 \
                            int32 \
                            int64 \
                            is \
                            mixin \
                            namespace \
                            not \
                            null \
                            or \
                            out \
                            override* \
                            private \
                            property* \
                            protected \
                            return \
                            set* \
                            shared* \
                            super* \
                            switch \
                            this* \
                            true \
                            try \
                            typedef \
                            uint \
                            uint8 \
                            uint16 \
                            uint32 \
                            uint64 \
                            void \
                            while \
                            xor");

    editor->SetScrollWidthTracking(true);         // Track horizontal scroll width
    editor->SetUseVerticalScrollBar(false);       // Hide vertical scrollbar completely

    // Enable margin for line numbers (margin 0)
    editor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    editor->SetMarginWidth(0, 25); // Adjust width as needed

    editor->SetMarginSensitive(0, false); // Disable click interaction if not needed

    wxFont font(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Inter");
    editor->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    editor->StyleClearAll(); // Apply the font to all styles

    // Basic styles
    editor->StyleSetForeground(wxSTC_C_COMMENT, *wxGREEN);
    editor->StyleSetForeground(wxSTC_C_COMMENTLINE, *wxGREEN);
    editor->StyleSetForeground(wxSTC_C_NUMBER, *wxBLUE);
    editor->StyleSetForeground(wxSTC_C_STRING, *wxRED);
    editor->StyleSetForeground(wxSTC_C_WORD, *wxBLUE);
    editor->StyleSetBold(wxSTC_C_WORD, true);
    editor->StyleSetForeground(wxSTC_C_OPERATOR, *wxBLACK);

    // Enable autocomplete settings
    editor->AutoCompSetIgnoreCase(true);      // Optional: case-insensitive
    editor->AutoCompSetAutoHide(true);        // Auto-hide if no match
    editor->AutoCompSetDropRestOfWord(true);  // Replace rest of word when selected
    editor->AutoCompSetSeparator(' ');        // Separator for keyword list

    wxString autoCompleteList = "and \
                        abstract* \
                        auto \
                        bool \
                        break \
                        case \
                        cast \
                        catch \
                        class \
                        const \
                        continue \
                        default \
                        delete* \
                        do \
                        double \
                        else \
                        enum \
                        explicit* \
                        external* \
                        false \
                        final* \
                        float \
                        for \
                        from* \
                        funcdef \
                        function* \
                        get* \
                        if \
                        import \
                        in \
                        inout \
                        int \
                        interface \
                        int8 \
                        int16 \
                        int32 \
                        int64 \
                        is \
                        mixin \
                        namespace \
                        not \
                        null \
                        or \
                        out \
                        override* \
                        private \
                        property* \
                        protected \
                        return \
                        set* \
                        shared* \
                        super* \
                        switch \
                        this* \
                        true \
                        try \
                        typedef \
                        uint \
                        uint8 \
                        uint16 \
                        uint32 \
                        uint64 \
                        void \
                        while \
                        xor \
                        Start() \
                        Update()";

    // When typing
    editor->Bind(wxEVT_STC_CHARADDED, [=](wxStyledTextEvent& evt)
    {
        char ch = evt.GetKey();
        if (std::isalnum(ch))
        {
            int pos = editor->GetCurrentPos();
            int start = editor->WordStartPosition(pos, true);
            int len = pos - start;

            if (len > 0)
            {
                wxString currentWord = editor->GetTextRange(start, pos).Lower();

                wxString filteredList;
                wxStringTokenizer tokenizer(autoCompleteList, " "); // assuming words are space-separated

                while (tokenizer.HasMoreTokens())
                {
                    wxString token = tokenizer.GetNextToken();
                    if (token.Lower().StartsWith(currentWord))
                        filteredList += token + " ";
                }

                if (!filteredList.IsEmpty())
                    editor->AutoCompShow(len, filteredList.Trim());
            }
        }

        unsave();
    });

    // When pressing backspace or delete
    editor->Bind(wxEVT_KEY_DOWN, [=](wxKeyEvent& event)
    {
        int keyCode = event.GetKeyCode();

        if (keyCode == WXK_BACK)
        {
            unsave();
        }
        else if (keyCode == WXK_DELETE)
        {
            unsave();
        }

        event.Skip(); // Let other handlers process the key
    });

    saveButton->Bind(wxEVT_BUTTON, [=](wxCommandEvent& event)
    {
        save();
    });

    Enable(false);
}

ScriptPane::~ScriptPane()
{
    //(*Destroy(ScriptPane)
    //*)
}

void ScriptPane::openScript(wxString name, wxString fullPath)
{
    // Check if previous script already saved
    if (!scriptSaved)
    {
        wxMessageDialog dlg(
            this,
            "This script has unsaved changes. Do you want to save before closing?",
            "Unsaved Script",
            wxYES_NO | wxCANCEL | wxICON_WARNING
        );

        int result = dlg.ShowModal();

        if (result == wxID_YES)
        {
            // Save the script
            return;  // proceed to close
        }
        else if (result == wxID_NO)
        {
            // Proceed
        }
        else
        {
            // Cancel close
            return;
        }
    }

    wxString content;
    wxFile file(fullPath);

    if (file.IsOpened())
    {
        file.ReadAll(&content);
    }
    else
    {
        wxLogError("Failed to open file: %s", fullPath);
        return;
    }

    scriptFullPath = fullPath;
    scriptName = name;
    scriptSaved = true;

    refreshScriptName();

    editor->SetText(content);
}

void ScriptPane::refreshScriptName()
{
    if (scriptSaved)
    {
        fileName->SetLabel(scriptName);
        saveButton->Enable(false);
    }
    else
    {
        fileName->SetLabel(scriptName + "*");
        saveButton->Enable(true);
    }
}

void ScriptPane::unsave()
{
    // Set it as unsaved
    if (scriptSaved)
    {
        scriptSaved = false;
        refreshScriptName();
    }
}

void ScriptPane::save()
{
    // Compile first, using the script manager from Kemene lib
}

