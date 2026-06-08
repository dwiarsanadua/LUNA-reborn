--[[
  Quest 326: [B Class] Giant Elimination
  Level Required: 49
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [325]
  Rewards: EXP=269078
]]

local fsm = require('fsm_engine')

local quest_326 = fsm:new({
    id = 326,
    name = "[B Class] Giant Elimination",
    level_required = 49,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {325},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 326: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 326: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 326: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 39,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 326: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 326: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 326: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 326: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 326: Rewards given")
                self:give_rewards({
                    exp = 269078,
                    items = {
                        { item_id = 232, count = 1 },
                        { item_id = 232, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_326