
// Beispiel-Endpoint für Suchanfragen
app.get('/api/search', async (req, res) => {
  try {
    const { query, index, field } = req.query;
    console.log("query:", query);
    console.log("index:", index);
    console.log("field:", field);

    if (!index) {
      return res.status(400).json({ error: 'Index parameter is required.' });
    }

    let searchBody: any = {};

    if (query && field) {
      searchBody = {
        query: {
          wildcard: { [field as string]: `*${query as string}*` }
        }
      };
    } else {
      return res.status(400).json({ error: 'Invalid request. Query and field required.' });
    }

    // OpenSearch-Abfrage
    const response = await client.search({
      index: index as string,
      body: searchBody
    });

    res.json(response.body.hits.hits);
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Fehler bei der Suche', details: error });
  }
});

app.get('/api/match_all', async (req, res) => {
  try {
    const { index } = req.query;
    console.log("index:", index);
    if (!index) {
      return res.status(400).json({ error: 'Index parameter is required.' });
    }

    let searchBody: any = {};
    searchBody = { query: { match_all: {} } };

    // OpenSearch-Abfrage
    const response = await client.search({
      index: index as string,
      size: 1000,
      body: searchBody
    });

    res.json(response.body.hits.hits);
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Fehler bei der Suche', details: error });
  }
});

app.get('/api/agr', async (req, res) => {
  try {
    const { index } = req.query;
    if (!index) {
      return res.status(400).json({ error: 'Index parameter is required.' });
    }

    // Aggregations-Abfrage
    const response = await client.search({
      index: index as string,
      body: {
        aggs: {
          alter_nach_name: {
            terms: {
              field: "Directory.keyword",  // Gruppierung nach dem Feld "name"
              order: { "total_size": "desc" }, // Sortierung nach Namen (aufsteigend)
            },
            aggs: {
              total_size: {
                sum: { field: "FileSize" },  // Summe des Feldes "alter" pro Gruppe
              }
            }
          }
        }
      }
    });

    console.log(response.body.aggregations.alter_nach_name)
    // Ergebnisse extrahieren
    const aggregations = response.body.aggregations.alter_nach_name.buckets;
    const ergebnis = aggregations.map((bucket: any) => ({
      name: bucket.key,
      total_size: bucket.total_size.value
    }));

    res.json(ergebnis);
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Fehler bei der Aggregation', details: error });
  }
});

app.post('/api/os', async (req, res) => {
  try{
    const response = await client.search(req.body)
    res.json(response)
  } catch(error){
    console.error(error)
    res.status(500).json({eror: "Error in api/os.", details:error})
  }
})