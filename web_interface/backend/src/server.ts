import express from 'express';
import cors from 'cors';
import { Client } from '@opensearch-project/opensearch';
import dotenv from 'dotenv';
import * as child from 'child_process'
import { opensearchQueries } from "./types/requests.js"

dotenv.config();

const app = express();
const port = 3000;

// CORS für Frontend erlauben
app.use(cors({
  origin: 'http://localhost:5173'
}));
app.use(express.json());

// initialize opensearch client
const client = new Client({
  node: process.env.OPENSEARCH_URL || 'http://localhost:9200/clean_test',
  auth: {
    username: process.env.OPENSEARCH_USERNAME || 'admin',
    password: process.env.OPENSEARCH_PASSWORD || 'admin'
  },
  ssl: {
    rejectUnauthorized: false
  }
});


const parseBoolean = (value: string | undefined): boolean => {
  if (value === undefined) return false; // Default-Wert
  return value.toLowerCase() === 'true';
};

app.get('/api/filters', async (req,res) => {
  try{
    
    //microscopeType?: string; group?: string; newerThan?: Date; olderThan?: Date
    const query = req.query;
    const filters = {
      microscopeType: query.microscopeType as string | undefined,
      group: query.group as string | undefined,
      newerThan: query.newerThan ? new Date(query.newerThan as string) : undefined,
      olderThan: query.olderThan ? new Date(query.olderThan as string) : undefined
    }

    const osQuery = opensearchQueries.queryWithFilter(filters)
    const response = await client.search({
      body: osQuery
    })

    res.json(response)

  } catch(err){
    console.log(err)
    res.status(500).json({err: "Server side error while searching with filters.", details:err })
  }
})

app.get('/api/spawn', async (req,res) => {
  try{
    child.exec("touch itworks")
    res.status(200).json({status: "Started Crawl."})
  } catch(err){
    console.log(err)
    res.status(500).json({err: "Error in spawning Process", details:err})
  }
})

app.get('/api/sum', async (req, res) => {
  try{
    console.log("Request received.")
    const { index, field, sum } = req.query;
    if (!index || !field || !sum) return res.status(400).json({error: "index, field and sum are required!"})
    const response = await client.search({
      index: index as string,
      body: {
        aggs: {
          alter_nach_name: {
            terms: {
              field: field as string + ".keyword",  // Gruppierung nach dem Feld "name"
              order: { "total_size": "desc" }, // Sortierung nach Namen (aufsteigend)
            },
            aggs: {
              total_size: {
                sum: { field: sum as string },  // Summe des Feldes "alter" pro Gruppe
              }
            }
          }
        }
      }
    });
    // console.log(response.body.aggregations.alter_nach_name)
    const aggregations = response.body.aggregations.alter_nach_name.buckets;
    const ergebnis = aggregations.map((bucket: any) => ({
      folder: bucket.key,
      FileSize: bucket.total_size.value
    }));
    console.log(ergebnis)
    res.json(ergebnis)
  } catch (error) {
    console.error(error);
    res.status(500).json({error: 'Fehler bei test.', details:error});
  }
});

app.get('/api/count', async (req,res) => {
  try{
    const {index, field} = req.query;
    if (!index || !field) return res.status(400).json({error: 'Index and Field required.'})
    const response = await client.search({
      index: index as string,
      body: {
        size: 0,
        aggs: {
          namen_häufigkeit: {
            terms: {
              field: field as string + ".keyword",
              size: 100,
              order: { _count: "desc" }
            }
          }
        }
      }
    })
    const data = response.body.aggregations.namen_häufigkeit.buckets
    console.log(data)
    res.json(data)
  } catch (error) {
    console.error(error)
    res.status(500).json({error: 'Error in count', details:error})
  }
})

// Server starten
app.listen(port, () => {
  console.log(`Backend läuft auf http://localhost:${port}`);
});