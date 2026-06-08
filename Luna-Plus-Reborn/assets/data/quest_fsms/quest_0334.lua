--[[
  Quest 334: [B Class] Cyclops Elimination
  Level Required: 54
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [333]
  Rewards: EXP=414585
]]

local fsm = require('fsm_engine')

local quest_334 = fsm:new({
    id = 334,
    name = "[B Class] Cyclops Elimination",
    level_required = 54,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {333},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 334: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 334: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 334: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 37,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 334: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 334: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 334: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 334: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 334: Rewards given")
                self:give_rewards({
                    exp = 414585,
                    items = {
                        { item_id = 240, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_334