--[[
  Quest 386: [A Class] Threat in the Cave
  Level Required: 74
  NPC Start: 95, NPC Complete: 95
  Prerequisites: [384]
  Rewards: EXP=1618141
]]

local fsm = require('fsm_engine')

local quest_386 = fsm:new({
    id = 386,
    name = "[A Class] Threat in the Cave",
    level_required = 74,
    npc_start = 95,
    npc_complete = 95,
    prerequisites = {384},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 386: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 386: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 386: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 90,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 386: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 91,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 386: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 386: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 386: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 386: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 386: Rewards given")
                self:give_rewards({
                    exp = 1618141,
                    items = {
                        { item_id = 30000503, count = 1 },
                        { item_id = 30000312, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_386