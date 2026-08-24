import type { reqFiltersType } from "./types_zod.js"
import { mappings } from "./mappings.js"


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
            size: 10000,
            query: {
                bool:{
                    must: []
                }
            } 
        }
        if (filters.microscopeType){

            const fields = mappings.MicroscopeTypeFieldnames
            const values = mappings.MicroscopeType[filters.microscopeType as keyof typeof mappings.MicroscopeType]
 
            query.query.bool.must.push({
                bool: {
                should: fields.map(field => ({
                    terms: {
                    [field]: values
                    }
                })),
                minimum_should_match: 1
                }
            })
                
        }
        if (filters.group && filters.group !== ""){
            query.query.bool.must.push({
                match: {"Group.keyword": filters.group}
            })
        }
        if (filters.newerThan){
            query.query.bool.must.push({
                range: {
                    "LastWrite": {
                        "gte": filters.newerThan.toISOString()
                    }
                }
            })
        }
        if (filters.olderThan){
            if(query.query.bool.must.range.LastWrite){
                query.bool.must.range.LastWrite.push({
                    "lte": [filters.olderThan]
                })
            } else {
                query.query.bool.must.push({
                    range: {
                        "LastWrite": {
                            "lte": [filters.olderThan]
                        }
                    }
                })
            }
        }
        return query
    }
}
