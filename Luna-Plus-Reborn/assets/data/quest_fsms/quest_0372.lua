--[[
  Quest 372: [A Class] Hiding in Plain Sight
  Level Required: 61
  NPC Start: 94, NPC Complete: 94
  Prerequisites: [351]
  Rewards: EXP=549177
]]

local fsm = require('fsm_engine')

local quest_372 = fsm:new({
    id = 372,
    name = "[A Class] Hiding in Plain Sight",
    level_required = 61,
    npc_start = 94,
    npc_complete = 94,
    prerequisites = {351},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 372: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 372: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 372: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 65,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 372: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 66,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 372: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 372: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 372: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 372: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 372: Rewards given")
                self:give_rewards({
                    exp = 549177,
                    items = {
                        { item_id = 30000503, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_372