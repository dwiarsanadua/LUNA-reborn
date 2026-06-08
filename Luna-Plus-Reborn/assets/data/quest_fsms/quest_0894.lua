--[[
  Quest 894: Frozen Heart Sculpture
  Level Required: 98
  NPC Start: 535, NPC Complete: 535
  Rewards: EXP=8938143, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_894 = fsm:new({
    id = 894,
    name = "Frozen Heart Sculpture",
    level_required = 98,
    npc_start = 535,
    npc_complete = 535,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 894: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 894: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 894: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 894: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 894: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 894: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 894: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 894: Rewards given")
                self:give_rewards({
                    exp = 8938143,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_894