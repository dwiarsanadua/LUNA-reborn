--[[
  Quest 824: For the Completion
  Level Required: 75
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [823]
  Rewards: EXP=1579838, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_824 = fsm:new({
    id = 824,
    name = "For the Completion",
    level_required = 75,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {823},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 824: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 824: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 824: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 259,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 824: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 824: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 824: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 824: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 824: Rewards given")
                self:give_rewards({
                    exp = 1579838,
                    gold = 41009,
                    items = {
                        { item_id = 30000685, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_824