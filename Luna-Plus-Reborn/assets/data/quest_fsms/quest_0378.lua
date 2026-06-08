--[[
  Quest 378: [A Class] Alert Level Blue
  Level Required: 61
  NPC Start: 95, NPC Complete: 95
  Prerequisites: [351]
  Rewards: EXP=549177
]]

local fsm = require('fsm_engine')

local quest_378 = fsm:new({
    id = 378,
    name = "[A Class] Alert Level Blue",
    level_required = 61,
    npc_start = 95,
    npc_complete = 95,
    prerequisites = {351},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 378: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 378: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 378: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 76,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 378: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 109,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 378: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 378: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 378: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 378: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 378: Rewards given")
                self:give_rewards({
                    exp = 549177,
                    items = {
                        { item_id = 30000503, count = 1 },
                        { item_id = 30000302, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_378