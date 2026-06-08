--[[
  Quest 332: [B Class] Outraged Gryphon Elimination
  Level Required: 52
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [331]
  Rewards: EXP=347459
]]

local fsm = require('fsm_engine')

local quest_332 = fsm:new({
    id = 332,
    name = "[B Class] Outraged Gryphon Elimination",
    level_required = 52,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {331},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 332: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 332: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 332: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 87,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 332: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 332: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 332: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 332: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 332: Rewards given")
                self:give_rewards({
                    exp = 347459,
                    items = {
                        { item_id = 238, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_332