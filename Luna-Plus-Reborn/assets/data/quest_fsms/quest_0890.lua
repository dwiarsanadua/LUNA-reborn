--[[
  Quest 890: Pirates of the Pier
  Level Required: 97
  NPC Start: 535, NPC Complete: 536
  Rewards: EXP=8266754, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_890 = fsm:new({
    id = 890,
    name = "Pirates of the Pier",
    level_required = 97,
    npc_start = 535,
    npc_complete = 536,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 890: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 890: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 890: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 292,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 890: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 890: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 890: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 890: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 890: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 890: Rewards given")
                self:give_rewards({
                    exp = 8266754,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_890