///////////////////////////////////////////////////////////////////////////////
// Name:        generic/accel.cpp
// Purpose:     generic implementation of wxAcceleratorTable class
// Author:      Robert Roebling
// Modified:    VZ pn 31.05.01: use typed lists, Unicode cleanup, Add/Remove
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "accel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ACCEL

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/list.h"
#endif // WX_PRECOMP

#include "wx/accel.h"

#include <ctype.h>

// ----------------------------------------------------------------------------
// wxAccelList: a list of wxAcceleratorEntries
// ----------------------------------------------------------------------------

WX_DECLARE_LIST(wxAcceleratorEntry, wxAccelList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAccelList);

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class wxAccelRefData : public wxObjectRefData
{
public:
    wxAccelRefData()
    {
        m_accels.DeleteContents(TRUE);
    }

    wxAccelRefData(const wxAccelRefData& data)
        : wxObjectRefData()
    {
        m_accels.DeleteContents(TRUE);
        m_accels = data.m_accels;
    }

    wxAccelList m_accels;
};

// macro which can be used to access wxAccelRefData from wxAcceleratorTable
#define M_ACCELDATA ((wxAccelRefData *)m_refData)


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxAcceleratorTable ctors
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable, wxObject)

wxAcceleratorTable::wxAcceleratorTable()
{
}

wxAcceleratorTable::wxAcceleratorTable(int n, wxAcceleratorEntry entries[])
{
    m_refData = new wxAccelRefData;

    for ( int i = 0; i < n; i++ )
    {
        const wxAcceleratorEntry& entry = entries[i];

        int keycode = entry.GetKeyCode();
        if ( wxIslower(keycode) )
            keycode = wxToupper(keycode);

        M_ACCELDATA->m_accels.Append(new wxAcceleratorEntry(entry.GetFlags(),
                                                            keycode,
                                                            entry.GetCommand()));
    }
}

wxAcceleratorTable::~wxAcceleratorTable()
{
}

bool wxAcceleratorTable::Ok() const
{
    return m_refData != NULL;
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable updating
// ----------------------------------------------------------------------------

void wxAcceleratorTable::Add(const wxAcceleratorEntry& entry)
{
    AllocExclusive();

    if ( !m_refData )
    {
        m_refData = new wxAccelRefData;
    }

    M_ACCELDATA->m_accels.Append(new wxAcceleratorEntry(entry));
}

void wxAcceleratorTable::Remove(const wxAcceleratorEntry& entry)
{
    AllocExclusive();

    wxAccelList::Node *node = M_ACCELDATA->m_accels.GetFirst();
    while ( node )
    {
        const wxAcceleratorEntry *entryCur = node->GetData();

        if ( *entryCur == entry )
        {
            M_ACCELDATA->m_accels.DeleteNode(node);

            return;
        }

        node = node->GetNext();
    }

    wxFAIL_MSG(_T("deleting inexistent accel from wxAcceleratorTable"));
}

// ----------------------------------------------------------------------------
// wxAcceleratorTable: find a command for the given key press
// ----------------------------------------------------------------------------

const wxAcceleratorEntry *
wxAcceleratorTable::GetEntry(const wxKeyEvent& event) const
{
    if ( !Ok() )
    {
        // not an error, the accel table is just empty
        return NULL;
    }

    wxAccelList::Node *node = M_ACCELDATA->m_accels.GetFirst();
    while ( node )
    {
        const wxAcceleratorEntry *entry = node->GetData();

        // is the key the same?
        if ( event.m_keyCode == entry->GetKeyCode() )
        {
            int flags = entry->GetFlags();

            // now check flags
            if ( (((flags & wxACCEL_CTRL) != 0) == event.ControlDown()) &&
                 (((flags & wxACCEL_SHIFT) != 0) == event.ShiftDown()) &&
                 (((flags & wxACCEL_ALT) != 0) ==
                    (event.AltDown() || event.MetaDown())) )
            {
                return entry;
            }
        }

        node = node->GetNext();
    }

    return NULL;
}

wxMenuItem *wxAcceleratorTable::GetMenuItem(const wxKeyEvent& event) const
{
    const wxAcceleratorEntry *entry = GetEntry(event);

    return entry ? entry->GetMenuItem() : NULL;
}

int wxAcceleratorTable::GetCommand(const wxKeyEvent& event) const
{
    const wxAcceleratorEntry *entry = GetEntry(event);

    return entry ? entry->GetCommand() : -1;
}

wxObjectRefData *wxAcceleratorTable::CreateRefData() const
{
    return new wxAccelRefData;
}

wxObjectRefData *wxAcceleratorTable::CloneRefData(const wxObjectRefData *data) const
{
    return new wxAccelRefData(*(wxAccelRefData *)data);
}

#endif // wxUSE_ACCEL

