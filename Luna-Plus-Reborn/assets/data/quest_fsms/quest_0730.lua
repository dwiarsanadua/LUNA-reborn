--[[
  Quest 730: [Quest] Clues?
  Level Required: 52
  NPC Start: 119, NPC Complete: 406
  Prerequisites: [729]
  Rewards: EXP=347459
]]

local fsm = require('fsm_engine')

local quest_730 = fsm:new({
    id = 730,
    name = "[Quest] Clues?",
    level_required = 52,
    npc_start = 119,
    npc_complete = 406,
    prerequisites = {729},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 730: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 730: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 730: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 395,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 730: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 730: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 406,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 730: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 730: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 406,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 730: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 730: Rewards given")
                self:give_rewards({
                    exp = 347459,
                    items = {
                        { item_id = 360, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_730