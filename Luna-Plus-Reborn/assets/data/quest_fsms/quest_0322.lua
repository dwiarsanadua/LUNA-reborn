--[[
  Quest 322: [B Class] Red Feather Harpy Elimination
  Level Required: 47
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [321]
  Rewards: EXP=230763
]]

local fsm = require('fsm_engine')

local quest_322 = fsm:new({
    id = 322,
    name = "[B Class] Red Feather Harpy Elimination",
    level_required = 47,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {321},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 322: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 322: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 322: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 322: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 322: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 322: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 322: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 322: Rewards given")
                self:give_rewards({
                    exp = 230763,
                    items = {
                        { item_id = 228, count = 1 },
                        { item_id = 228, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_322