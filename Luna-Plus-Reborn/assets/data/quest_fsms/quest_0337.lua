--[[
  Quest 337: [B Class] Giant Wanderer Elimination
  Level Required: 58
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [335]
  Rewards: EXP=574238
]]

local fsm = require('fsm_engine')

local quest_337 = fsm:new({
    id = 337,
    name = "[B Class] Giant Wanderer Elimination",
    level_required = 58,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {335},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 337: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 337: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 337: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 97,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 337: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 337: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 337: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 337: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 337: Rewards given")
                self:give_rewards({
                    exp = 574238,
                    items = {
                        { item_id = 243, count = 1 },
                        { item_id = 243, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_337