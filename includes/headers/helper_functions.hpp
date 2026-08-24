// Source - https://stackoverflow.com/a/58237530
// Posted by Gulrak, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-11, License - CC BY-SA 4.0
template <typename TP>
std::string time_to_string(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now() + system_clock::now());
    std::time_t tt = system_clock::to_time_t(sctp);
    std::tm *local_time = std::localtime(&tt);
    std::stringstream buffer;
    // buffer << std::put_time(local_time, "%A, %d %B %Y %H:%M");
    buffer << std::put_time(local_time, "%Y-%m-%dT%H:%M:%S%z");
    return buffer.str();
}