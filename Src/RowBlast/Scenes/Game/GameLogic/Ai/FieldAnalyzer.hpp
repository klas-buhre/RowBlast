#ifndef FieldAnalyzer_hpp
#define FieldAnalyzer_hpp

namespace RowBlast {
    class Field;
    
    class FieldAnalyzer {
    public:
        explicit FieldAnalyzer(const Field& field);
        
        float CalculateBurriedHolesAreaInVisibleRows() const;
        float CalculateBurriedHolesAreaInVisibleRowsWithGravity(int landedPieceId) const;
        float CalculateWellsAreaInVisibleRows() const;
        int CalculateNumTransitionsInVisibleRows() const;
        int CalculateNumCellsAccordingToBlueprintInVisibleRows() const;
        float CalculateBuildHolesAreaInVisibleRows() const;
        float CalculateBuildWellsAreaInVisibleRows() const;
        
    private:
        int CalculateNumTransitionsInColumns() const;
        int CalculateNumTransitionsInRows() const;
        
        const Field& mField;
    };
}

#endif
