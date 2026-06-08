--[[
  Quest 324: [B Class] Gryphon Elimination
  Level Required: 48
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [323]
  Rewards: EXP=249474
]]

local fsm = require('fsm_engine')

local quest_324 = fsm:new({
    id = 324,
    name = "[B Class] Gryphon Elimination",
    level_required = 48,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {323},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 324: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 324: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 324: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 324: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 324: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 324: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 324: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 324: Rewards given")
                self:give_rewards({
                    exp = 249474,
                    items = {
                        { item_id = 230, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_324