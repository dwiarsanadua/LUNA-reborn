--[[
  Quest 323: [B Class] Gryphon Elimination
  Level Required: 48
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [321]
  Rewards: EXP=249474
]]

local fsm = require('fsm_engine')

local quest_323 = fsm:new({
    id = 323,
    name = "[B Class] Gryphon Elimination",
    level_required = 48,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {321},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 323: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 323: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 323: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 323: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 323: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 323: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 323: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 323: Rewards given")
                self:give_rewards({
                    exp = 249474,
                    items = {
                        { item_id = 229, count = 1 },
                        { item_id = 229, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_323