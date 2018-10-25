#ifndef FieldAnalyzer_hpp
#define FieldAnalyzer_hpp

namespace RowBlast {
    class Field;
    
    class FieldAnalyzer {
    public:
        explicit FieldAnalyzer(const Field& field);
        
        float GetBurriedHolesAreaInVisibleRows() const;
        float GetWellsAreaInVisibleRows() const;
        int GetNumTransitionsInVisibleRows() const;
        int GetNumCellsAccordingToBlueprintInVisibleRows() const;
        float GetBuildHolesAreaInVisibleRows() const;
        float GetBuildWellsAreaInVisibleRows() const;
        
    private:
        int GetNumTransitionsInColumns() const;
        int GetNumTransitionsInRows() const;
        
        const Field& mField;
    };
}

#endif
