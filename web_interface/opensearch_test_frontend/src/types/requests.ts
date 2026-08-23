import type { reqFiltersType } from "./types_zod"

export const mappings = {
    MicroscopeType: {
        "Widefield": ["AxioScan 7", "AxioScan.Z1", "AxioZoom"],
        "Confocal": ["LSM710 Imager", "LSM780 Observer", "LSM880 Examiner", "LSM880 Imager", "LSM880 Observer", "LSM980 NIR", "Marianas Spinning Disk"]
    }
}

export const opensearchQueries = {
    aggregateByField: (groupField: string, sumField: string) => ({
        aggs: {
            [`group_by_${groupField}`]: {
                terms: {
                    field: `${groupField}.keyword`,
                    order: { "total_size": "desc" },
                },
                aggs: {
                    total_size: {
                        sum: { field: sumField },
                    }
                }
            },
        }
    }),
    queryWithFilter: (filters: reqFiltersType) => {
        const query: any= {
          query: {
            bool:{
                must: []
            }
          } 
        }
        if (filters.microscopeType){
            query.query.bool.must.push({
                match: {"FileType.keyword": [filters.microscopeType]}
            })
        }
        if (filters.group && filters.group !== ""){
            query.query.bool.must.push({
                match: {"FileGroupID.keyword": [filters.group]}
            })
        }
        if (filters.newerThan){
            query.query.bool.must.push({
                range: {
                    "FileModifyDate": {
                        "gte": [filters.newerThan]
                    }
                }
            })
        }
        if (filters.olderThan){
            if(query.query.bool.must.range.FileModifyDate){
                query.bool.must.range.FileModifyDate.push({
                    "lte": [filters.olderThan]
                })
            } else {
                query.query.bool.must.push({
                    range: {
                        "FileModifyDate": {
                            "lte": [filters.olderThan]
                        }
                    }
                })
            }
        }
    }
}
