--[[
  Quest 331: [B Class] Outraged Gryphon Elimination
  Level Required: 52
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [329]
  Rewards: EXP=347459
]]

local fsm = require('fsm_engine')

local quest_331 = fsm:new({
    id = 331,
    name = "[B Class] Outraged Gryphon Elimination",
    level_required = 52,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {329},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 331: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 331: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 331: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 331: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 331: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 331: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 331: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 331: Rewards given")
                self:give_rewards({
                    exp = 347459,
                    items = {
                        { item_id = 237, count = 1 },
                        { item_id = 237, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_331