--[[
  Quest 889: Dream to Fly
  Level Required: 96
  NPC Start: 535, NPC Complete: 535
  Rewards: EXP=7628195, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_889 = fsm:new({
    id = 889,
    name = "Dream to Fly",
    level_required = 96,
    npc_start = 535,
    npc_complete = 535,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 889: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 889: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 889: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 291,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 889: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 889: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 889: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 889: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 889: Rewards given")
                self:give_rewards({
                    exp = 7628195,
                    gold = 77721,
                    items = {
                        { item_id = 21000015, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_889