--[[
  Quest 446: Evidence of the Elimination
  Level Required: 55
  NPC Start: 119, NPC Complete: 119
  Prerequisites: [445]
  Rewards: EXP=361052
]]

local fsm = require('fsm_engine')

local quest_446 = fsm:new({
    id = 446,
    name = "Evidence of the Elimination",
    level_required = 55,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {445},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 446: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 446: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 446: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 446: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 94,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 446: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 446: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 446: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 446: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 446: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 446: Rewards given")
                self:give_rewards({
                    exp = 361052,
                    items = {
                        { item_id = 319, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_446